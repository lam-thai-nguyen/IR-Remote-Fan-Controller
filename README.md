# CONTROLLING FAN WITH INFRARED REMOTE CONTROL

<details>
  <summary>English</summary>
  <p>
  <hr>
  </p>
</details>

<details>
    <summary>Vietnamese</summary>
    - Sử dụng điều khiển quạt Panasonic để điều khiển động cơ điện một chiều<br>
    - Điều khiển gồm 5 nút: OFF/ON, SPEED, RHYTHM, OSCIL., TIMER<br>
    - Các tín hiệu được mã hóa như sau<br>
    <ul>
      <li>OFF/ON: 0x43430E7F</li>
      <li>SPEED: 0x54511082</li>
      <li>RHYTHM: 0x705C5422</li>
      <li>OSCIL.: 0xE6F839DE</li>
      <li>TIMER: 0x1E11AAFE</li>
    </ul>
    - Các chức năng<br>
    <ul>
      <li>OFF/ON: Bật/tắt động cơ</li>
      <li>SPEED: Tăng tốc độ (3 tốc độ)</li>
      <li>RHYTHM: Tốc độ tăng dần đến MAX_SPEED rồi lặp lại</li>
      <li>OSCIL.: Đổi chiều động cơ</li>
      <li>TIMER: Hẹn giờ tắt động cơ</li>
    </ul>
    - Một số lưu ý<br>
    <ul>
      <li>Các chức năng chỉ có tác dụng sau khi bấm ON</li>
      <li>Sau khi bấm OFF, các chức năng bị tắt (Ví dụ: timer trước đó sẽ không hoạt động khi bấm ON lại)</li>
    </ul>
    - Cần cải thiện<br>
    <ul>
      <li>Khi động cơ tắt do chế độ hẹn giờ, cần bấm ON 2 lần để khởi động lại, trong đó lần bấm ON thứ nhất là tắt nguồn</li>
      <li>--> Cải thiện bằng cách gửi ngược lại trạng thái "power" từ slave đến master, để lần bấm ON tiếp theo khởi động LED</li>
    </ul>
    <hr>
</details>

