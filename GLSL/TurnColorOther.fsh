#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

void main()
{
	gl_FragColor = texture2D(CC_Texture0, v_texCoord);
	
	//高光 颜色中减去黑色（或者理解成所有颜色乘以2）
	//gl_FragColor = vec4(mix(vec3(0.0, 0.0, 0.0), gl_FragColor.rgb, 2.0), gl_FragColor.a) * v_fragmentColor;
	
	//负色 白色减去当前颜色
	//gl_FragColor = vec4(vec3(1.0, 1.0, 1.0) - gl_FragColor.rgb, gl_FragColor.a) * v_fragmentColor;
	
	//对比效果
	// 使用一个灰色图作为基础图像，和彩色图混合。
	// 逐像素的将图片颜色差向灰度两边扩大，得到对比效果，
	//也就是1.0的颜色变成了1.5，0.5的颜色还是0.5, 0.0的颜色变成了-0.5 其他类推
	gl_FragColor = vec4(mix(vec3(0.5, 0.5, 0.5), gl_FragColor.rgb, 2.0), gl_FragColor.a) * v_fragmentColor;
	
	
}

