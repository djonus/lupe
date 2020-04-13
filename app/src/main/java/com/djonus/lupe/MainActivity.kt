package com.djonus.lupe

import android.content.Context
import android.media.AudioManager
import android.os.Bundle
import android.util.Log
import android.view.MotionEvent
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import com.djonus.lupe.utils.exponential
import com.djonus.lupe.utils.normalize
import kotlinx.android.synthetic.main.activity_main.*


class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        adjustDefaultStreamValue()

        val engineRef = createEngine()

        synth_pad.setOnTouchListener { view, event ->

            val x = view.normalizeFrequency(event.x)
            val y = view.normalizeAmplitude(view.height - event.y)

            tv_synth_input.text = "x:${event.x}\ny:${event.y}\n\nfrequency:$x\namplitude:$y"

            Log.d("LupeAct", "touch event: $event")

            val isTouchingPad = event.x >= 0 && event.x <= view.width
                    && event.y >= 0 && event.y <= view.height

            when (event.action) {
                MotionEvent.ACTION_UP -> stopSynth(engineRef)
                else -> when {
                    !isTouchingPad -> stopSynth(engineRef)
                    else -> playSynth(engineRef, x, y)
                }
            }
            false
        }

        val stopText = "stop"
        val playText = "play"
        b_play.text = playText
        b_play.setOnClickListener {
            b_play.text = when (b_play.text) {
                playText -> {
                    startPlayback(engineRef)
                    stopText
                }
                else -> {
                    stopPlayback(engineRef)
                    playText
                }
            }
        }

        val stopRecordText = "cut"
        val recordText = "record"
        b_record.text = recordText
        b_record.setOnClickListener {
            b_record.text = when (b_record.text) {
                recordText -> {
                    record(engineRef)
                    stopRecordText
                }
                else -> {
                    stopRecord(engineRef)
                    recordText
                }
            }
        }
    }

    external fun setDefaultStreamValues(defaultSampleRate: Int, defaultFramesPerBurst: Int)
    external fun createEngine(): Long
    external fun playSynth(engineRef: Long, x: Double, y: Double)
    external fun stopSynth(engineRef: Long)

    external fun startPlayback(engineRef: Long)
    external fun stopPlayback(engineRef: Long)
    external fun record(engineRef: Long)
    external fun stopRecord(engineRef: Long)

    private fun adjustDefaultStreamValue() {
        val myAudioMgr = getSystemService(Context.AUDIO_SERVICE) as AudioManager

        val sampleRateStr = myAudioMgr
            .getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE)

        val framesPerBurstStr = myAudioMgr
            .getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER)

        setDefaultStreamValues(sampleRateStr.toInt(), framesPerBurstStr.toInt())
    }

    private val frequencyRange: ClosedRange<Double> = 20.0..20000.0
    private val frequencyExponentialFactor = 2.0
    private val amplitudeRange: ClosedRange<Double> = 0.0..2.0
    private val amplitudeExponentialFactor = 2.0

    private fun View.normalizeFrequency(x: Float): Double =
        (0.0..width.toDouble())
            .normalize(frequencyRange, x)
            .exponential(frequencyExponentialFactor, frequencyRange)

    private fun View.normalizeAmplitude(y: Float): Double =
        (0.0..height.toDouble())
            .normalize(amplitudeRange, y)
            .exponential(amplitudeExponentialFactor, amplitudeRange)

    companion object {

        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}
