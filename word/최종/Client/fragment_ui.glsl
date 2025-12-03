#version 330 core

in vec3 Color;         // ���ؽ� ���̴����� ���޹��� ����
in vec2 TexCoords;     // �ؽ�ó ��ǥ

out vec4 FragColor;    // ���� ��� ����

uniform sampler2D texture1; // �ؽ�ó ���÷�
uniform bool isRed;         // UI ��� �÷���
uniform bool isTimer;       // Ÿ�̸� �÷���
uniform bool isTexture;     // �ؽ�ó �÷���

void main() {
    if (isRed) {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0); // ������
    } else if (isTimer) {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0); // ���
    } else if (isTexture) {
        FragColor = texture(texture1, TexCoords); // �ؽ�ó ���ø�
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // �⺻�� (������)
    }
}
