package com.djonus.lupe

import android.content.Context
import android.media.AudioManager
import android.os.Bundle
import android.util.Log
import android.view.MotionEvent
import android.view.View
import android.widget.SeekBar
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

        val multipliers = arrayOf(
            "x1/8" to 1.0/8,
            "x1/7" to 1.0/7,
            "x1/6" to 1.0/6,
            "x1/5" to 1.0/5,
            "x1/4" to 1.0/4,
            "x1/3" to 1.0/3,
            "x1/2" to 1.0/2,
            "x1" to 1.0,
            "x2" to 2.0,
            "x3" to 3.0,
            "x4" to 4.0,
            "x5" to 5.0,
            "x6" to 6.0,
            "x7" to 7.0,
            "x8" to 8.0
        )
        sb_1.max = multipliers.size - 1
        val defaultMultiplier = multipliers[7]
        sb_1.setProgress(7, true)
        tv_multiplier.text = defaultMultiplier.first
        sb_1.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                val multiplier = multipliers[progress]
                tv_multiplier.text = multiplier.first
                setTapeSizeMultiplier(engineRef, multiplier.second)
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
            }

        })

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
        val cursors = cursorData.asIterable().chunked(2).takeLast(loops.childCount).reversed()
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
    external fun setTapeSizeMultiplier(engineRef: Long, multiplier: Double)

    private fun adjustDefaultStreamValue() {
        val myAudioMgr = getSystemService(Context.AUDIO_SERVICE) as AudioManager

        val sampleRateStr = myAudioMgr
            .getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE)

        val framesPerBurstStr = myAudioMgr
            .getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER)

        setDefaultStreamValues(sampleRateStr.toInt(), framesPerBurstStr.toInt())
    }

    private val frequencyRange: ClosedRange<Double> = 20.0..20000.0
    private val frequencyExponentialFactor = 3.2
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
