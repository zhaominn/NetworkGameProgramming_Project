#version 330 core

// �Է� ������
in vec3 FragPos;   // ���� ���������� �����׸�Ʈ ��ġ (Vertex Shader���� ����)
in vec3 Normal;    // ������ ���� ���� (Vertex Shader���� ����)
in vec2 TexCoords; // �ؽ�ó ��ǥ (Vertex Shader���� ����)

// ��� ������
out vec4 FragColor; // ���� ��� ����

// ī�޶�� ���� ���� Uniform ����
uniform vec3 viewPos;      // ī�޶�(���) ��ġ
uniform vec3 lightPos;     // ������ ��ġ
uniform vec3 lightColor;   // ������ ����

// �ؽ�ó �� �ؽ�ó ���� �÷���
uniform sampler2D map_Ka;  // ȯ�汤 �ؽ�ó
uniform sampler2D map_Kd;  // ���ݻ籤 �ؽ�ó
uniform sampler2D map_Ks;  // �ݻ籤 �ؽ�ó
uniform bool hasKaTexture; // ȯ�汤 �ؽ�ó ���� �÷���
uniform bool hasKdTexture; // ���ݻ籤 �ؽ�ó ���� �÷���
uniform bool hasKsTexture; // �ݻ籤 �ؽ�ó ���� �÷���

// �⺻ ���� �Ӽ�
uniform vec3 Ka;           // �⺻ ȯ�汤 ���
uniform vec3 Kd;           // �⺻ ���ݻ籤 ���
uniform vec3 Ks;           // �⺻ �ݻ籤 ���
uniform float Ns;          // ��¦�� ���� (Shininess)

uniform bool isUI;          // UI ��� �÷���

void main() {
    // 1. ���� ���� ����ȭ
    vec3 norm = normalize(Normal);

    // 2. ���� ���� ���
    vec3 lightDir = normalize(lightPos - FragPos);

    // 3. ī�޶� ���� ���
    vec3 viewDir = normalize(viewPos - FragPos);

    // 4. **ȯ�汤 (Ambient) ���**
    vec3 ambientColor = hasKaTexture ? texture(map_Ka, TexCoords).rgb : Ka; // �ؽ�ó �Ǵ� �⺻��
    vec3 ambient = ambientColor * lightColor;

    // 5. **���ݻ籤 (Diffuse) ���**
    float diff = max(dot(norm, lightDir), 0.0); // Lambert's cosine law
    vec3 diffuseColor = hasKdTexture ? texture(map_Kd, TexCoords).rgb : Kd; // �ؽ�ó �Ǵ� �⺻��
    vec3 diffuse = diffuseColor * diff * lightColor;

    // 6. **�ݻ籤 (Specular) ���**
    vec3 reflectDir = reflect(-lightDir, norm); // �ݻ� ���� ���
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Ns); // Phong reflection model
    vec3 specularColor = hasKsTexture ? texture(map_Ks, TexCoords).rgb : Ks; // �ؽ�ó �Ǵ� �⺻��
    vec3 specular = specularColor * spec * lightColor;

    // 7. **���� ���� ���**
    vec3 result = ambient + diffuse + specular;

    // 8. ���� ���
    //FragColor = vec4(texture(map_Kd, TexCoords).rgb, 1.0);
    vec4 texColor = texture(map_Kd, TexCoords);
    if (texColor.a < 0.1)
        discard;
    FragColor = texColor;
}


 //FragColor = vec4(result, 1.0);
 //FragColor = vec4(Normal * 0.5 + 0.5, 1.0); // ������ (0, 1) ������ ��ȯ
 //FragColor = vec4(TexCoords, 0.0, 1.0); // UV ��ǥ�� �������� ���
 //FragColor = vec4(diff * lightColor, 1.0); // ������Ȯ��
 //FragColor = vec4(texture(map_Kd, TexCoords).rgb, 1.0); //�ؽ�ó ����� �̹����� �ٷγ�����
 //FragColor = vec4(ambient, 1.0); //���� ����� �������� Ȯ�� (ȯ�汤 �������� �������)
 //FragColor = vec4(diffuse, 1.0); //���ݻ籤(diffuse)�� ����Ͽ� ���� ����� �������� Ȯ��
 //FragColor = vec4(specular, 1.0); //�ݻ籤(specular)�� ����Ͽ� ���� ����� �������� Ȯ��
