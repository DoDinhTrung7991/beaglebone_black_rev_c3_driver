# BBB Timer4 PWM Driver

Dự án này là một Linux Platform Driver dùng để điều khiển Timer4 trên BeagleBone Black (AM335x) nhằm tạo xung PWM. 

## 🚀 Tính năng
- [cite_start]Điều khiển phần cứng Timer thông qua các thanh ghi (TCLR, TLDR, TMAR)[cite: 176, 177, 321, 322].
- [cite_start]Giao tiếp với User-space qua Character Device (`/dev/my_driver`)[cite: 191, 211, 241].
- [cite_start]Hỗ trợ IOCTL để ghi và đọc giá trị trực tiếp từ Kernel[cite: 194, 264].
- [cite_start]Quản lý tài nguyên an toàn với bộ API `devm_*` của Linux Kernel[cite: 224, 226, 311].

## 🛠 Cấu trúc thư mục
- [cite_start]`my_driver.c`: File chính xử lý logic driver và giao tiếp file hệ thống.
- [cite_start]`timer.c`: Xử lý cấu hình tần số và duty cycle cho Timer[cite: 304, 319, 320].
- [cite_start]`std_util.c`: Các hàm tiện ích đọc/ghi thanh ghi 32-bit[cite: 172, 176].
- `test_app.c`: Ứng dụng User-space để kiểm tra Driver.

## 📝 Ghi nhận (Credits)
Dự án này được phát triển dựa trên việc học tập và tham khảo mã nguồn từ:
- Cộng đồng Linux Kernel (Kernel Platform Driver mẫu).
- Tham khảo logic điều khiển Timer từ các nguồn hướng dẫn lập trình nhúng cho AM335x.
- Xin cảm ơn các tác giả đi trước đã chia sẻ mã nguồn mở để tôi có thể học hỏi và phát triển thêm.

## ⚖️ Giấy phép (License)
Dự án này được phát hành dưới giấy phép **GPLv2**.
