module CONTROL_UNIT (
  CLK,
  HREF,
  VSYNC,
  input_data,
  output_data,
  X_ADDR,
  Y_ADDR,
  w_en
);

input CLK;
input [7:0] input_data;
input HREF;
input VSYNC;

output reg [14:0] X_ADDR;
output reg [14:0] Y_ADDR;
output reg w_en;
output [7:0] output_data;

reg write = 0;
reg [7:0] part1;
reg [7:0] part2;

always @ (posedge CLK) begin
  if (HREF)
  begin
    if (write == 0)
    begin
      part1 <= input_data;
	   write <= 1;
	   w_en <= 0;
		X_ADDR <= X_ADDR;
    end
    else
	 begin
      part2 <= input_data;
	   write <= 0;
	   w_en <= 1;
		X_ADDR <= X_ADDR + 1;
    end
  end
  else
  begin
    w_en <= 0;
	 write <= 0;
	 X_ADDR <= 0;
  end
end

always @ (posedge VSYNC, negedge HREF) begin
	if (VSYNC) begin
		Y_ADDR <= 0;
	end
	else
	begin
		Y_ADDR <= Y_ADDR + 1;
	end
end


DOWNSAMPLE downsampler(
  .RGB565({part1, part2}),
  .RGB332(output_data)
);


//  if ((Y_ADDR > 20 && Y_ADDR < 140) && (X_ADDR > 20 && X_ADDR < 160)) begin
//    input_data <= 8'b10000100;
//  end else begin
//    input_data <= 8'b10000100;
//  end
  /*if (X_ADDR < 20)
  begin
    input_data <= 8'b11111111;
  end
  if (X_ADDR < 40 && X_ADDR > 19)
  begin
    input_data <= 8'b00000010;
  end
  if (X_ADDR < 60 && X_ADDR > 39)
  begin
    input_data <= 8'b00000100;
  end
  if (X_ADDR < 80 && X_ADDR > 59)
  begin
    input_data <= 8'b00001000;
  end
  if (X_ADDR < 100 && X_ADDR > 79)
  begin
    input_data <= 8'b00010000;
  end
  if (X_ADDR < 120 && X_ADDR > 99)
  begin
    input_data <= 8'b00100000;
  end
  if (X_ADDR < 140 && X_ADDR > 119)
  begin
    input_data <= 8'b01000000;
  end
  if (X_ADDR < 176 && X_ADDR > 139)
  begin
    input_data <= 8'b10000000;
  end
  */
  
endmodule
