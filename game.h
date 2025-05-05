#ifndef GAME_H
#define GAME_H

#define INITIAL_SPEED 3

// Cấu trúc Mouse đại diện cho chuột trong trò chơi
struct Mouse {
    int x, y;          // Vị trí chuột
    int dx = 0, dy = 0; // Tốc độ thay đổi vị trí chuột
    int speed = INITIAL_SPEED; // Tốc độ di chuyển chuột

    // Phương thức để di chuyển chuột theo hướng đã chỉ định
    void move() {
        x += dx;
        y += dy;
    }

    // Di chuyển chuột theo hướng Bắc (lên)
    void turnNorth() {
        dy = -speed; // Di chuyển lên trên
        dx = 0;
    }

    // Di chuyển chuột theo hướng Nam (xuống)
    void turnSouth() {
        dy = speed; // Di chuyển xuống dưới
        dx = 0;
    }

    // Di chuyển chuột theo hướng Tây (trái)
    void turnWest() {
        dy = 0;
        dx = -speed; // Di chuyển sang trái
    }

    // Di chuyển chuột theo hướng Đông (phải)
    void turnEast() {
        dy = 0;
        dx = speed; // Di chuyển sang phải
    }

    // Phương thức nhảy chuột (chỉ có thể nhảy khi đang ở mặt đất)
    void jump() {
        dy-=10;
    }

    // Áp dụng trọng lực cho chuột (giúp chuột rơi trở lại mặt đất)
    void applyGravity() {
        if (y < SCREEN_HEIGHT - 168) { // Giới hạn độ cao của chuột (chiều cao của chuột là 168)
            dy += 1; // Gia tốc trọng lực
        } else {
            dy = 0; // Chuột không rơi qua mặt đất
            y = SCREEN_HEIGHT - 168; // Đặt lại vị trí chuột ở mặt đất
        }
    }

    void increaseSpeed();
    void decreaseSpeed();
};

// Hàm render chuột với sprite (texture) thay vì hình chữ nhật
// Chú ý: Cần truyền vào texture của chuột
void render(const Mouse& mouse, const Graphics& graphics) {
    SDL_Rect filled_rect;
    filled_rect.x = mouse.x;
    filled_rect.y = mouse.y;
    filled_rect.w = 182; // Chiều rộng của chuột
    filled_rect.h = 168; // Chiều cao của chuột

    // Bạn có thể chọn màu sắc chuột nếu muốn (ví dụ màu xanh lá)
    SDL_SetRenderDrawColor(graphics.renderer, 0, 255, 0, 255); // Màu xanh lá
    SDL_RenderFillRect(graphics.renderer, &filled_rect); // Vẽ hình chữ nhật
}

// Hàm kiểm tra điều kiện game over (chuột ra ngoài màn hình)
bool gameOver(const Mouse& mouse) {
    return mouse.x < 0 || mouse.x >= SCREEN_WIDTH ||  // Chuột ra ngoài chiều rộng
           mouse.y < 0 || mouse.y >= SCREEN_HEIGHT;   // Chuột ra ngoài chiều cao
}

// Thêm khả năng thay đổi tốc độ chuột
// Tăng tốc độ chuột
void Mouse::increaseSpeed() {
    speed += 1; // Tăng tốc độ chuột
}

// Giảm tốc độ chuột
void Mouse::decreaseSpeed() {
    if (speed > 1) speed -= 1; // Giảm tốc độ chuột nếu không nhỏ hơn 1
}

#endif // GAME_H
