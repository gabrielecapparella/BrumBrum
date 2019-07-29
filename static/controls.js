document.onselectstart = function() {
    return false;
}

$(document).ready(function(){
	var wheel_offset = $("#wheel").offset();
	var current_x = 150;
	var current_y = 150;
	
	$("#dot").css("margin-left", 150-11+"px");
	$("#dot").css("margin-top", 150-11+"px");

	$("#wheel").mousedown(function() {
		$(document).mousemove(function(e) {
			rel_x = parseInt(e.pageX-wheel_offset.left);
			rel_y = parseInt(e.pageY-wheel_offset.top);
			move(rel_x, rel_y);
		});
	});

	$(document).mouseup(function() {
		$(document).off("mousemove");
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
		if (x>300) x=300;
		if (y>300) y=300;

		$.ajax({
			url:'/move', 
			data:{
				x: x-150,
				y: y-150
			},
		});
		current_x = x;
		current_y = y;
		$("#dot").css("margin-left", x-11+"px");
		$("#dot").css("margin-top", y-11+"px");
	}
});