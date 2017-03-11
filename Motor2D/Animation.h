#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "p2Point.h"
#include "SDL/include/SDL.h"
#include <vector>

using namespace std;

class Animation
{
public:
	float speed;
	bool loop;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	vector<SDL_Rect> frames;
	iPoint pivot;

private:
	float current_frame;
	int loops;

public:
	Animation() : frames(0), speed(1.0f), current_frame(0), loop(true), loops(0), flip(SDL_FLIP_NONE)
	{}

	Animation(const Animation& a) : frames(a.frames), speed(a.speed), current_frame(0), loop(a.loop), loops(0), pivot(a.pivot), flip(a.flip)
	{}

	void SetFrames(int x, int y, int sprite_w, int sprite_h, int num_frames, int margin = 0)
	{
		for (int i = 0; i < num_frames; i++)
		{
			frames.push_back({ x + (sprite_w + margin) * i, y, sprite_w, sprite_h });
		}
	}

	SDL_Rect& getCurrentFrame()
	{
		current_frame += speed;
		if(current_frame >= frames.size())
		{
			current_frame = (loop) ? 0.0f : frames.size() - 1;
			loops++;
		}

		return frames[(int)current_frame];
	}

	const SDL_Rect& PeekCurrentFrame() const
	{
		return frames[(int)current_frame];
	}

	bool Finished()
	{
		return loops > 0;
	}

	void Reset()
	{
		current_frame = 0;
		loops = 0;
	}

	int CurrentFrame() const
	{
		return (int)current_frame;
	}

	float getCurrentFrame()const
	{
		return current_frame;
	}

	bool isOver()
	{
		return (current_frame + 1 >= frames.size() && loop == false);
	}

	void setAnimation(int x, int y, int w, int h, int frameNum, int margin)
	{
		for (int i = 0; i < frameNum; ++i)
		{
			frames.push_back({ x + (w + margin) * i, y, w, h });
		}
	}

	void setAnimations(int x, int y, int w, int h, int framesPerRow, int framesPerCol, int frameNum)
	{
		for (int i = 0; i < framesPerCol; ++i)
		{
			for (int j = 0; j < framesPerRow; ++j)
			{
				frames.push_back({ x + (w * j), y + (h * i), w, h });
			}
		}
		current_frame = 0.0f;
	}
};

#endif _ANIMATION_H_