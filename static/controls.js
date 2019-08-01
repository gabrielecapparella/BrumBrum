document.onselectstart = function() {
    return false;
}

$(document).ready(function(){
	var following_mouse = false;
	var wheel_offset = $("#wheel").offset();
	var wheel_size = 200;
	var current_x = wheel_size/2;
	var current_y = wheel_size/2
	var last_x = wheel_size/2;
	var last_y = wheel_size/2;

	$("#wheel").mousedown(function() {
		following_mouse = true;
		$(document).mousemove(function(e) {
			rel_x = parseInt(e.pageX-wheel_offset.left);
			rel_y = parseInt(e.pageY-wheel_offset.top);
			move(rel_x, rel_y);
		});
	});

	$(document).mouseup(function() {
		$(document).off("mousemove");
		move(wheel_size/2, wheel_size/2);
	});

	$(document).keyup(function(e){
		switch(e.key) {
			case "w":
			case "ArrowUp":
				move(current_x, current_y-5);
				break;
			case "a":
			case "ArrowLeft":
				move(current_x-5, current_y);
				break;
			case "s":
			case "ArrowDown":
				move(current_x, current_y+5);
				break;
			case "d":
			case "ArrowRight":
				move(current_x+5, current_y);
				break;
		}
	});

	function move(x, y) {
		if (x<0) x=0;
		if (y<0) y=0;
		if (x>wheel_size) x=wheel_size;
		if (y>wheel_size) y=wheel_size;


		current_x = x;
		current_y = y;

		diff = Math.abs(current_x-last_x)+Math.abs(current_y-last_y)

		// I only send the request if the dot has been moved by at least 5px
		if (diff>=5) { 
			$.ajax({
				url:'/motors', 
				data:{
					x: x-wheel_size/2,
					y: -y+wheel_size/2
				},
			});
			last_x = x;
			last_y = y;
		}

		$("#dot").css("margin-left", x-11+"px");
		$("#dot").css("margin-top", y-11+"px");
	}
});