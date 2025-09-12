#pragma once
#include <iostream>

class Mode {
public:
	static Mode* currentInstance;  // ���� Ȱ��ȭ�� Mode �ν��Ͻ��� �����ϴ� ���� ������

	virtual void init() = 0; // ��� ���۽� ����
	virtual void keyboard(unsigned char key, int x, int y) = 0;
	virtual void specialKey(int key, int x, int y) = 0;
	virtual void specialKeyUp(int key, int x, int y) = 0;
	virtual void mouseClick(int button, int state, int x, int y) = 0;
	virtual void draw_model() = 0;
	virtual void draw_bb() = 0;
	virtual void finish() = 0;

	virtual ~Mode() {}  // ���� �Ҹ���
};