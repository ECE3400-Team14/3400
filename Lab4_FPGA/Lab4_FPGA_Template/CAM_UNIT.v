module CAM_UNIT(
  CLK,
  HREF,
  input_data,
  output_data,
  w_en
);

input CLK;
input HREF;
input [7:0] input_data;
output [7:0] output_data;
output reg w_en;

reg write = 0;
reg [7:0] part1;
reg [7:0] part2;

always @ (posedge CLK)
begin
  if (HREF)
  begin
    if (write == 0)
    begin
      part1 <= input_data;
	   write <= 1;
	   w_en <= 0;
    end
    else
	 begin
      part2 <= input_data;
	   write <= 0;
	   w_en <= 1;
    end
  end
  else
  begin
    w_en <= 0;
	 write <= 0;
  end
end


DOWNSAMPLE downsampler(
  .RGB565({part1, part2}),
  .RGB332(output_data)
);

endmodule
