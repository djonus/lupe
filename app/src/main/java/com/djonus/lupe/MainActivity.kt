package com.djonus.lupe

import android.content.Context
import android.media.AudioManager
import android.os.Bundle
import android.util.Log
import android.view.MotionEvent
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_main.*
import kotlin.math.roundToInt


class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        tv_result.text = stringFromJNI()

        adjustDefaultStreamValue()

        val engineRef = createEngine()

        synth_pad.setOnTouchListener { view, event ->

            val x = event.x.roundToInt()
            val y = event.y.roundToInt()

            Log.d("LupeAct", "event: ${event.action}")

            when (event.action) {
                MotionEvent.ACTION_UP -> stopSynth(engineRef)
                else -> playSynth(engineRef, x, y)
            }
            true
        }
    }

    external fun stringFromJNI(): String
    external fun setDefaultStreamValues(defaultSampleRate: Int, defaultFramesPerBurst: Int)
    external fun createEngine(): Long
    external fun playSynth(engineRef: Long, x: Int, y: Int)
    external fun stopSynth(engineRef: Long)

    private fun adjustDefaultStreamValue() {
        val myAudioMgr = getSystemService(Context.AUDIO_SERVICE) as AudioManager

        val sampleRateStr = myAudioMgr
            .getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE)

        val framesPerBurstStr = myAudioMgr
            .getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER)

        setDefaultStreamValues(sampleRateStr.toInt(), framesPerBurstStr.toInt())
    }

    companion object {

        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}
