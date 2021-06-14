package com.example.android2;

import android.content.Context;
import android.graphics.Color;
import androidx.appcompat.widget.AppCompatTextView;

import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;
import android.view.Gravity;

public class MyView extends AppCompatTextView implements OnGestureListener, OnDoubleTapListener
{
	
	private GestureDetector gestureDetector;

	public MyView(Context context)
	{
		super(context);

		setText("Hello World !!!");
		setTextSize(64);
		setTextColor(Color.rgb(0, 255, 0));
		setBackgroundColor(Color.rgb(0, 0, 0));
		setGravity(Gravity.CENTER);

		gestureDetector = new GestureDetector(context, this, null, false);
		gestureDetector.setOnDoubleTapListener(this);
	} 

	@Override
	public boolean onTouchEvent(MotionEvent event)
	{
		// code
		int eventaction = event.getAction();
		if(!gestureDetector.onTouchEvent(event))
			super.onTouchEvent(event);
		return (true);

	}

	@Override
	public boolean onDoubleTap(MotionEvent e)
	{
		setText("Double Tap");

		return (true);
	}

	@Override
	public boolean onDoubleTapEvent(MotionEvent e)
	{
		return (true);
	}

	@Override
	public boolean onSingleTapConfirmed(MotionEvent e)
	{
		setText("Single Tap");

		return (true);
	}

	@Override
	public boolean onDown(MotionEvent e)
	{
		return (true);
	}

	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY)
	{
		return (true);
	}

	@Override
	public void onLongPress(MotionEvent e)
	{
		setText("Long Press");
	}

	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
	{
		setText("Scroll");
		System.exit(0);
		return (true);
	}

	@Override
	public void onShowPress(MotionEvent e)
	{
	}

	@Override
	public boolean onSingleTapUp(MotionEvent e)
	{
		return (true);
	}
}

