package com.example.hello;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import androidx.appcompat.widget.AppCompatTextView;
import android.graphics.Color;
import android.view.Gravity;

import android.content.pm.ActivityInfo;

import android.view.View;

public class MainActivity extends AppCompatActivity 
{
	

    @Override
    protected void onCreate(Bundle savedInstanceState) 
    {
		AppCompatTextView myTextView = new AppCompatTextView(this);

        super.onCreate(savedInstanceState);
        getWindow().getDecorView().setBackgroundColor(Color.rgb(0, 0, 0));

		myTextView.setText("Hellow World !!!");
		myTextView.setTextSize(64);
		myTextView.setTextColor(Color.rgb(0, 255, 0));
		myTextView.setGravity(Gravity.CENTER);
		myTextView.setBackgroundColor(Color.rgb(0, 0, 0));

        setContentView(myTextView);
    }
}

