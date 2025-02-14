// By qweasdd
#include "common.h"

uniform float4 screen_res;		// Screen resolution (x-Width,y-Height, zw - 1/resolution)

//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
p_screen main(v2p_screen I)
{
	p_screen O;
	
	{
		I.HPos.xy += 0.5f;
		O.hpos.x = I.HPos.x * screen_res.z * 2 - 1;
		O.hpos.y = (I.HPos.y * screen_res.w * 2 - 1)*-1;
		O.hpos.zw = I.HPos.zw;
	}
    O.tc0 = I.tc0;
  
    return O; 
}