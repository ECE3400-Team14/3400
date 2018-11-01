`define SCREEN_WIDTH 176
`define SCREEN_HEIGHT 144
`define NUM_BARS 3
`define BAR_HEIGHT 48

module IMAGE_PROCESSOR (
	PIXEL_IN,
	CLK,
	VGA_PIXEL_X,
	VGA_PIXEL_Y,
	VGA_VSYNC_NEG,
	RESULT
);


//=======================================================
//  PORT declarations
//=======================================================
input	[7:0]	PIXEL_IN;
input 		CLK;

input [9:0] VGA_PIXEL_X;
input [9:0] VGA_PIXEL_Y;
input			VGA_VSYNC_NEG;

output reg [7:0] RESULT = 8'b0;

reg [14:0] blue;
reg [14:0] red;

always @ (posedge CLK) begin
	if (VGA_PIXEL_X == 0 && VGA_PIXEL_Y == 0) begin
		blue <= 0;
		red <= 0;
	end
	else begin
		if (VGA_PIXEL_X < 176 && VGA_PIXEL_Y < 144) begin
			if (PIXEL_IN[7] == 1 && PIXEL_IN[1:0] == 0 && PIXEL_IN[4:3] == 0) begin
				red <= red + 1;
			end
			else begin
				red <= red;
			end
			if (PIXEL_IN[7:6] == 0 && PIXEL_IN[1] == 1 && PIXEL_IN[4:3] == 0) begin
				blue <= blue + 1;
			end
			else begin
				blue <= blue;
			end
		end
	end
end

always @ (*) begin
	if (red > 12672) begin
		RESULT <= 8'b00000001;
	end
	else begin
		if (blue > 12672) begin
			RESULT <= 8'b00000010;
		end
		else begin
			RESULT <= 8'b0;
		end
	end
end

endmodule