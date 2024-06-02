#include "graphic.h"
#include "../../assets/letters.xpm"
#include "../../assets/mouse.xpm"
#include "../../assets/white_letters.xpm"
#include "../../drivers/rtc/rtc.h"
#include "../../assets/title.xpm"
#include "../../drivers/mouse/mouse.h"

#define MAX_MESSAGE_NUM 10000
#define DARK_MODE_HOUR 12


char **messages = NULL;
int num_messages = 0;
bool seen[1024] = {false};
char *joined_messages = NULL;
char *chatter_username = NULL;
char *username = NULL;
bool in_menu = true;
DATE date;
int current_hour = 0;

void alloc_mem_messages_buffer() {
    messages = malloc(MAX_MESSAGE_NUM * sizeof(char *));
    joined_messages = malloc(MAX_MESSAGE_NUM * sizeof(char));
}

void clear_buffer(void* buffer){
    memset(buffer, 0, vram_size);
}

int flip_buffer() {
    reg86_t reg;
    memset(&reg, 0, sizeof(reg));

    reg.ah = 0x4F;
    reg.al = 0x07;
    reg.bl = 0;
    reg.cx = 0;
    reg.dx = current_buffer * mode_info.YResolution;
    reg.intno = 0x10;

    if(sys_int86(&reg) != OK) return 1;
    current_buffer = (current_buffer + 1) % VG_NUM_BUFFER;

    return 0;

}

int startVideoMode(u16_t mode){
    reg86_t reg;
    memset(&reg, 0, sizeof(reg));
    reg.intno = 0x10;
    reg.ax = 0x4F02;
    reg.bx = mode | BIT(14);
    if(sys_int86(&reg) != OK) return 1;
    return 0;
}

int exitVideoMode(){
    reg86_t reg;
    memset(&reg, 0, sizeof(reg));
    reg.intno = 0x10;
    reg.ah = 0x00;
    reg.al = 0x03;
    if(sys_int86(&reg) != OK) return 1;
    return 0;
}

int initFrameBuffer(u16_t mode){
    memset(&mode_info, 0, sizeof(mode_info));
    if(vbe_get_mode_info(mode, &mode_info) != 0) return 1;

    bytesPerPixel = (mode_info.BitsPerPixel + 7) / 8;
    x_res = mode_info.XResolution;
    y_res = mode_info.YResolution;

    vram_size = x_res * y_res * bytesPerPixel;

    struct minix_mem_range address;
    address.mr_base = mode_info.PhysBasePtr;
    address.mr_limit = address.mr_base + VG_NUM_BUFFER * vram_size;

    if(sys_privctl(SELF, SYS_PRIV_ADD_MEM, &address) != 0) return 1;

    current_buffer = 0;
    for(u8_t buffer = 0; buffer < VG_NUM_BUFFER; buffer++){
        buffers[buffer] = vm_map_phys(SELF, (void*) (address.mr_base + buffer * vram_size), vram_size);
        clear_buffer(buffers[buffer]);
        buffer_changed[buffer] = false;
    }


	if (rtc_get_date_decimal(&date) != 0) {
		printf("Error fetching current date...\n");
		return 1;
	}

    current_hour = date.hours;
    printf("%d\n",current_hour);

    return 0;
}

int drawPixel(u16_t x, u16_t y, u32_t color, void* buffer){
    u8_t* position = (u8_t*) buffer + ((y * x_res + x) * bytesPerPixel);
    memcpy(position, &color, bytesPerPixel);
    return 0;
}

int drawLine(u16_t x, u16_t y, u16_t width, u32_t color, void* buffer){
    for(u32_t i = 0; i < width; i++){
        if(drawPixel(x + i, y, color, buffer) != 0) return 1;
    }
    return 0;
}

int drawRectangle(u16_t x, u16_t y, u16_t width, u16_t height, u32_t color, void* buffer){
    for(u32_t i = 0; i < height; i++){
        if(drawLine(x, y + i, width, color, buffer) != 0) return 1;
    }
    return 0;
}

int draw_xpm(xpm_map_t xpm, u16_t x, u16_t y, void* buffer) {
    xpm_image_t img;
    u32_t *map = (u32_t*) xpm_load(xpm, XPM_8_8_8_8, &img);
    for(int i = 0; i < img.height; i++){
        for(int j = 0; j < img.width; j++){
            if(*map != 0x00ff00) drawPixel(x + j, y + i, *map, buffer);
            map++;
        }
    }
    return 0;
}

int update_buffer(int *text, uint index, bool blink,u16_t x_mouse,u16_t y_mouse){
    u16_t x = X_AXIS(0.05);
    u16_t y = Y_AXIS(0.9);
    u8_t next_buffer = (current_buffer + 1) % VG_NUM_BUFFER;
    formatBackground(buffers[next_buffer]);
    if(num_messages > 0) {
        drawMessages(buffers[next_buffer],blink,x_mouse,y_mouse);
    }
    for(uint i = 0; i < index; i++){
        if(text[i] != -1) {
            if(x + 12 > X_AXIS(0.7)){
                x = X_AXIS(0.05);
                y += 10;
                draw_xpm((xpm_map_t) a_xpm[text[i]], x, y, buffers[next_buffer]);
                x += 12;
            }
            else{
                draw_xpm((xpm_map_t) a_xpm[text[i]], x, y, buffers[next_buffer]);
                x += 12;
            }
        }
    }
    if(blink && !in_menu) draw_xpm((xpm_map_t) mouse_xpm[1], x,y, buffers[next_buffer]);
    draw_mouse(x_mouse,y_mouse);
    buffer_changed[next_buffer] = true;
    flip_buffer();
    return 0;
}

void update_message_buffer(char *message, bool is_yours) {
    char *date = rtc_get_date_str();
    if(message[0] >= 'a') {
        return;
    }
    strcat(joined_messages,message);
    if(message[strlen(message) - 1] == '.') {
        strcat(joined_messages,"\0"); 

        char *r = malloc(sizeof(char) * (strlen(joined_messages) + 100));
        //strcpy(r,is_yours ? username : chatter_username);
        strcpy(r,joined_messages);

        if(username == NULL && is_yours && strlen(r) > 2){
            username = r;
            joined_messages[0] = '\0';
            return;
        }
        if(chatter_username == NULL && !is_yours && strlen(r) > 2){
            chatter_username = r;
            joined_messages[0] = '\0';
            return;
        }

        char m[1024];
        uint current_index = 0;
        char *user = is_yours ? username : chatter_username;

        while(current_index < strlen(date)) {
            m[current_index] = date[current_index];
            current_index++;
        }
        for(uint i = 0; i < strlen(user); i++) {
            m[current_index + i] = user[i];
        }
        current_index += strlen(user);
        for(uint i = 0; i < strlen(r); i++) {
            m[current_index + i] = r[i];
        }

        m[strlen(joined_messages) + strlen(date) + strlen(user)] = '\0';

        //strcat(rr,date);
        //strcat(rr,sender);

        strcpy(r,m);

        joined_messages[0] = '\0';
        messages[num_messages] = r;
        num_messages++;
    } 
 }

void free_message_buffer() {
    for(int i = 0; i < num_messages; i++) {
        free(messages[i]);
    }
    free(messages);
    free(joined_messages);
}

 int drawMessages(void* buffer,bool blink,u16_t x_mouse, u16_t y_mouse){

    u16_t x = X_AXIS(0.05);
    u16_t y = Y_AXIS(0.83) - (10 * LINES_PER_MESSAGE(num_messages - 1  < 0 ? 0 : num_messages - 1));

    for(int i = num_messages - 1; i >= 0; i--){
        u16_t initialY = y;
        struct collision_box new_message_box = {x - X_AXIS(0.02),y,X_AXIS(0.01),X_AXIS(0.01)};
        if(mouse_collision(x_mouse,y_mouse,new_message_box)) seen[i] = true;
        if(blink && !seen[i]) drawRectangle(x - X_AXIS(0.02),y,X_AXIS(0.01),Y_AXIS(0.01),VG_COLOR_RED,buffer);
        for(uint j = 0; j < strlen(messages[i]); j++){
            int index = check_index(messages[i][j]);
            draw_xpm(current_hour > DARK_MODE_HOUR ? (xpm_map_t) white_xpm[index] : (xpm_map_t) a_xpm[index], x, y, buffer);
            
            if(x + 12 > X_AXIS(0.725)) {
                x = X_AXIS(0.05);
                y += 10;
            } else {
                x += 12;
            }
        }
        x = X_AXIS(0.05);
        if(i - 1 >= 0) y = initialY - (LINES_PER_MESSAGE(i-1)) * 12;
        if(y < Y_AXIS(0.075)) return 0;
    }
    return 0;
 }

int check_index(char character){
    if(character >= 'A' && character <= 'Z'){
        return character - 'A';
    }
    else if(character >= '0' && character <= '9'){
        return character - '0' + 28;
    }
    if(character == '-') return 39;
    if(character == '[') return 40;
    if(character == ']') return 41;

    return character == ' ' ? 26 : 27;
}

int formatBackground(void* buffer){
    drawRectangle(0, 0, x_res, y_res, current_hour > DARK_MODE_HOUR ? VG_COLOR_BLACK : VG_COLOR_WHITE, buffer);
    drawRectangle(0, 0, x_res, Y_AXIS(0.075), VG_COLOR_BLUE, buffer);

    drawRectangle(X_AXIS(0.75), Y_AXIS(0.075), X_AXIS(0.25), Y_AXIS(0.925), current_hour > DARK_MODE_HOUR ? VG_COLOR_LIGHT_BLUE : VG_COLOR_GRAY, buffer);
    drawRectangle(X_AXIS(0.95), Y_AXIS(0.0175), Y_AXIS(0.04), Y_AXIS(0.04), VG_COLOR_RED, buffer);
    if(!in_menu) {
        drawRectangle(X_AXIS(0.05), Y_AXIS(0.9), X_AXIS(0.65), Y_AXIS(0.05), current_hour > DARK_MODE_HOUR ? VG_COLOR_LIGHT_BLUE : VG_COLOR_GRAY, buffer);  
    } else {
        draw_title(X_AXIS(0.05),Y_AXIS(0.1),buffer);
        drawRectangle(X_AXIS(0.1),Y_AXIS(0.4),X_AXIS(0.2),Y_AXIS(0.1),VG_COLOR_BLUE,buffer);
        drawRectangle(X_AXIS(0.4),Y_AXIS(0.4),X_AXIS(0.2),Y_AXIS(0.1),VG_COLOR_RED,buffer);
    }

    if(!in_menu) {
        char *add_username = "";
        if(username == NULL) add_username = "DEFINE YOUR USERNAME";
        if(username != NULL && chatter_username == NULL) add_username = "WAITING FOR OTHER USER";
        for(uint i = 0; i < strlen(add_username);i++) {
            int index = check_index(add_username[i]);
            draw_xpm(current_hour > DARK_MODE_HOUR ? (xpm_map_t) white_xpm[index] : (xpm_map_t) a_xpm[index],X_AXIS(0.05) + (12 * i),Y_AXIS(0.85),buffer);
        }

    }

    int j = 0;
    for(int i = 0; i < Y_AXIS(0.02); i++){
        int x_1 = X_AXIS(0.956) + i;
        int y = Y_AXIS(0.025) + j;
        int x_2 = (X_AXIS(0.956) + Y_AXIS(0.02)) - i;
        drawPixel(x_1, y, VG_COLOR_WHITE, buffer);
        drawPixel(x_2, y, VG_COLOR_WHITE, buffer);
        j++;
    }

    if(username != NULL && chatter_username != NULL){
        draw_names(username, X_AXIS(0.8), Y_AXIS(0.1), buffer);
        draw_names(chatter_username, X_AXIS(0.8), Y_AXIS(0.1) + (strlen(username) / 7 + 1)*10, buffer);
    }
    return 0;
}

int draw_names(char* name, int x, int y, void* buffer){
    for(uint i = 0; i < strlen(name) - 1; i++){
        int index = check_index(name[i]);
        draw_xpm(current_hour > DARK_MODE_HOUR ? (xpm_map_t) white_xpm[index] : (xpm_map_t) a_xpm[index], x, y, buffer);
        if(x + 12 > X_AXIS(0.9)) {
            x = X_AXIS(0.8);
            y += 10;
         } else {
            x += 12;
        }
    }
    return 0;
}

int draw_mouse(int x,int y){
    draw_xpm(current_hour > DARK_MODE_HOUR ? (xpm_map_t) mouse_xpm[2] : (xpm_map_t) mouse_xpm[0] ,(u16_t)x,(u16_t)y,buffers[current_buffer]);
    return 0;
}

int reset_screen() {
    drawRectangle(0,0,x_res,y_res,0x0,buffers[current_buffer]);
    return 0;
}

int draw_title(int x,int y,void *buffer) {
    for(int i = 0; i < 4; i++) {
        draw_xpm((xpm_map_t) characters_xpm[i],x + (24 * i),y,buffer);
    }
    return 0;
}
