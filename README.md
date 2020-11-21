READ ME
=================================================================================================================================================
HƯỚNG DẪN THIẾT LẬP TRANG TÍNH GOOGLE SPREADSHEET VÀ MẠCH WIFI ESP32

1.Đăng nhập vào Google Drive, chọn New=>Google Spreadsheet.
2.Tools=>Script Editor, copy toàn bộ code từ file spreadsheetcode.txt vào script
3.Publish=>Deploy as webapp, đặt tên cho project VD:esp32, đợi đến khi hộp thoại xuất hiện, chỉnh "Who has access to app:" từ "Only myself" sang "Anyone, even anonymous" sau đó chọn Deploy
4.Google sẽ yêu cầu sự cho phép của người dùng để thực hiện file, chọn Review permission, sau đó chọn tài khoản Google đang dùng, rồi click Advanced, Go to Untitle project, Allow
5.Hộp thoại mới hiện lên cung cấp cho bạn URL của trang tính, thiết lập trang tính đến đây là xong.
6.Mở file esp32+buffer+dualcore trong Arduino IDE, sửa const char * ssid = "tên wifi hiện có"; và const char * password = "mật khẩu wifi hiện có";.
7.Copy URL có được lúc trước dán vào String GOOGLE_SCRIPT_ID = "URL của bạn";
8.Upload code từ Arduino IDE lên ESP32
===================================================================================================================================================
