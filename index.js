//

const PORT = 3000;								//Đặt địa chỉ Port được mở ra để tạo ra chương trình mạng Socket Server
     
var http = require('http') 						//#include thư viện http - Tìm thêm về từ khóa http nodejs trên google nếu bạn muốn tìm hiểu thêm. Nhưng theo kinh nghiệm của mình, Javascript trong môi trường NodeJS cực kỳ rộng lớn, khi bạn bí thì nên tìm hiểu không nên ngồi đọc và cố gắng học thuộc hết cái reference (Tài liêu tham khảo) của nodejs làm gì. Vỡ não đó!
var socketio = require('socket.io')				//#include thư viện socketio
 
var ip = require('ip');
var app = http.createServer();					//#Khởi tạo một chương trình mạng (app)
var io = socketio(app);							//#Phải khởi tạo io sau khi tạo app!
app.listen(PORT);								// Cho socket server (chương trình mạng) lắng nghe ở port 3000
console.log("Server nodejs chay tai dia chi: " + ip.address() + ":" + PORT)
 
//Khi có mệt kết nối được tạo giữa Socket Client và Socket Server
io.on('connection', function(socket) {	
    //hàm console.log giống như hàm Serial.println trên Arduino
    console.log("Connected"); //In ra màn hình console là đã có một Socket Client kết nối thành công.
    
    //định nghĩa một mảng 1 chiều có 2 phần tử: true, false.
    //+ true: Air-conditioner is active
    //+ false: Air-conditioner is standby
    var led = [true, false] 
    var temp
    //Tạo một chu kỳ nhiệm vụ sẽ chạy lại sau mỗi 200ms
    var interval1 = setInterval(function() {
        //đảo trạng thái của mảng led, đảo cho vui để ở Arduino nó nhấp nháy cho vui.
        for (var i = 0; i < led.length; i++) {
            led[i] = !led[i]
        }
        
        //Cài đặt chuỗi JSON, tên biến JSON này là json 
        var json = {
            "status": led, //có một phần tử là "led", phần tử này chứa giá trị của mảng led.;
            "temp": temp 
        }
        socket.emit('Control', json) //Gửi lệnh LED với các tham số của của chuỗi JSON
        console.log("send LED")//Ghi ra console.log là đã gửi lệnh LED
    }, 200)//200ms
    
    //Khi socket client bị mất kết nối thì chạy hàm sau.
    socket.on('disconnect', function() {
        console.log("disconnect") 	//in ra màn hình console cho vui
        clearInterval(interval1)		//xóa chu kỳ nhiệm vụ đi, chứ không xóa là cái task kia cứ chạy mãi thôi đó!
    })
});