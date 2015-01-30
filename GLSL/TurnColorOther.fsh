#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

void main()
{
	gl_FragColor = texture2D(CC_Texture0, v_texCoord);
	
	//�߹� ��ɫ�м�ȥ��ɫ����������������ɫ����2��
	//gl_FragColor = vec4(mix(vec3(0.0, 0.0, 0.0), gl_FragColor.rgb, 2.0), gl_FragColor.a) * v_fragmentColor;
	
	//��ɫ ��ɫ��ȥ��ǰ��ɫ
	//gl_FragColor = vec4(vec3(1.0, 1.0, 1.0) - gl_FragColor.rgb, gl_FragColor.a) * v_fragmentColor;
	
	//�Ա�Ч��
	// ʹ��һ����ɫͼ��Ϊ����ͼ�񣬺Ͳ�ɫͼ��ϡ�
	// �����صĽ�ͼƬ��ɫ����Ҷ��������󣬵õ��Ա�Ч����
	//Ҳ����1.0����ɫ�����1.5��0.5����ɫ����0.5, 0.0����ɫ�����-0.5 ��������
	gl_FragColor = vec4(mix(vec3(0.5, 0.5, 0.5), gl_FragColor.rgb, 2.0), gl_FragColor.a) * v_fragmentColor;
	
	
}

