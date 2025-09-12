#version 330 core

layout (location = 0) in vec3 in_Position;  // ���� ��ġ
layout (location = 1) in vec3 in_Normal;    // ���� ����
layout (location = 2) in vec2 aTexCoords;  // �ؽ�ó ��ǥ (�ɼ�)
layout (location = 4) in vec3 in_Color;     // ���� ���� (�߰�)

out vec3 Color;        // �����׸�Ʈ ���̴��� ���޵� ����
out vec2 TexCoords;    // �ؽ�ó ��ǥ�� �����׸�Ʈ ���̴��� ����

uniform mat4 model;              // �� ��ȯ ���

uniform bool isRed;          // UI ��� �÷���
uniform bool isTimer;        // UI ��� �÷���
uniform bool isTexture;      // �ؽ�ó Ȱ��ȭ �÷���

void main() {
    // ȭ�� ���� ��ġ ����

    // �÷��׿� ���� ���� �Ǵ� �ؽ�ó ��ǥ ����
    if (isRed) {
        gl_Position = vec4(in_Position.xy, 0.0, 1.0);

        Color = in_Color;
        TexCoords = vec2(0.0, 0.0); // �⺻ ��ǥ (�ʿ� ���� ��� 0���� ����)
    } else if (isTimer) {
        gl_Position = vec4(in_Position.xy, 0.0, 1.0);

        Color = in_Color;
        TexCoords = vec2(0.0, 0.0); // �⺻ ��ǥ
    } else if (isTexture) {
        gl_Position = model * vec4(in_Position, 1.0);

        TexCoords = aTexCoords; // �ؽ�ó ��ǥ�� �ùٸ��� ����
        Color = vec3(1.0, 1.0, 1.0); // �ؽ�ó ��忡���� �⺻ ���
    } else {
        // �⺻�� ����
        TexCoords = vec2(0.0, 0.0);
        Color = vec3(0.0, 0.0, 0.0); // ������
    }
}
