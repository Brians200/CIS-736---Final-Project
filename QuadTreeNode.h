#pragma once

class QuadTreeNode
{
public:
	QuadTreeNode(void);

	~QuadTreeNode(void);

	float AboveTopLeftCorner;
	float AboveTopRightCorner;
	float AboveBottomLeftCorner;
	float AboveBottomLeftCorner;
	float BelowTopLeftCorner;
	float BelowTopRightCorner;
	float BelowBottomLeftCorner;
	float BelowBottomRightCorner;

private:


};
