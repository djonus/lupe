package com.djonus.lupe

import android.content.Context
import android.media.AudioManager
import android.os.Bundle
import android.util.Log
import android.view.MotionEvent
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.get
import com.djonus.lupe.utils.exponential
import com.djonus.lupe.utils.normalize
import kotlinx.android.synthetic.main.activity_main.*
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch


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
        val playText = "start"
        b_2.text = playText
        b_2.setOnClickListener {
            b_2.text = when (b_2.text) {
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

        val stopRecordText = "stop recording"
        val recordText = "record"
        b_1.text = recordText
        b_1.setOnClickListener {
            b_1.text = when (b_1.text) {
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

        b_3.text = "save candidate"
        b_3.setOnClickListener {
            saveCandidate(engineRef)
        }

        b_4.text = "drop last loop"
        b_4.setOnClickListener {
            dropLastLoop(engineRef)
        }

        GlobalScope.launch {
            while (true) {
                delay(33)
                val cursorData = getLoopCursors(engineRef)
                displayCursors(cursorData)
            }
        }
    }

    private fun displayCursors(cursorData: IntArray) {
        val width = loops.width
        val cursors = cursorData.asIterable().chunked(2)
        cursors.forEachIndexed { index, cursor ->
            val progress = cursor[1].toFloat() / cursor[0].toFloat()
            loops[index].translationX = width * progress
        }
        for (i in cursors.size until loops.childCount) {
            loops[i].translationX = 0f
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
    external fun saveCandidate(engineRef: Long)
    external fun dropLastLoop(engineRef: Long)
    external fun getLoopCursors(engineRef: Long): IntArray

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
