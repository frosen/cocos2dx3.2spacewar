#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

//-------------------------------

uniform vec2 lightPos; //������������λ�� ������ı���
uniform int sampleNum; //��������
uniform float intensity; //ǿ��
uniform float exposure; //�ع�� 0.5����
uniform float decay; //��ǿ˥���� 0.95����

//-------------------------------

void main(void)
{
    vec4 color = texture2D(CC_Texture0, v_texCoord); //ԭʼ�����ȡ�ĵ�ǰ����rgba
    
    vec2 newTex = vec2(v_texCoord.x, v_texCoord.y); //������λ��
    
    vec2 deltaTexCoord = (newTex - lightPos) / float(sampleNum); //���յ�ָ��������ʸ���������ղ������г�һС��ʸ��
    
      float illuminationdecay = 1.0; //��ǿ˥��
      vec4 sampleColor = vec4(0.0); //�������ɫ��
      
      for(int i = 0; i < sampleNum; ++i)
      {
            //ȡ���������յ�ƫ��һ�������ȵĵ�
    	    newTex.x = newTex.x - deltaTexCoord.x;
    	    newTex.y = newTex.y - deltaTexCoord.y;
    
    	    //ȡ�˵����ɫ
    	    sampleColor = texture2D(CC_Texture0, newTex);
    	    
    	    //��ɫ����ϵ����˥���Ⱥ�ǿ�ȣ�
    	    sampleColor *= (illuminationdecay * intensity);
    
    	    //�ۼӵ�������ɫ��
    	    color += sampleColor;
    
    	    //˥��
    	    illuminationdecay *= illuminationdecay * decay; 
    }
    
    color *= exposure; //�ع��
    
    gl_FragColor = color * v_fragmentColor;
}