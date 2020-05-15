package com.djonus.lupe

import android.content.Context
import android.media.AudioManager
import android.os.Bundle
import android.util.Log
import android.view.MotionEvent
import android.view.View
import android.view.ViewGroup
import android.widget.SeekBar
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.get
import androidx.lifecycle.lifecycleScope
import com.djonus.lupe.utils.*
import kotlinx.android.synthetic.main.activity_main.*
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.io.File


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

        b_start.setOnClickListener { startPlayback(engineRef) }
        b_stop.setOnClickListener { stopPlayback(engineRef) }

        b_record.setTouchEventListener(MotionEvent.ACTION_DOWN) {
            record(engineRef)
            refreshTrackDetails(engineRef)
        }
        b_stop_record.setTouchEventListener(MotionEvent.ACTION_DOWN) {
            stopRecord(engineRef)
            refreshTrackDetails(engineRef)
        }

        b_add.setTouchEventListener(MotionEvent.ACTION_DOWN) {
            saveCandidate(engineRef)
            refreshTrackDetails(engineRef)
        }

        b_remove.setOnClickListener {
            getTrackDetails(engineRef).dropLast(2).lastOrNull()?.let {
                deleteLoop(engineRef, it)
                refreshTrackDetails(engineRef)
            }
        }

        val multipliers = arrayOf(
            "x1/8" to 1.0 / 8,
            "x1/7" to 1.0 / 7,
            "x1/6" to 1.0 / 6,
            "x1/5" to 1.0 / 5,
            "x1/4" to 1.0 / 4,
            "x1/3" to 1.0 / 3,
            "x1/2" to 1.0 / 2,
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

        lifecycleScope.launch {
            while (true) {
                delay(33)
                val cursorData = getLoopCursors(engineRef)
                displayCursors(cursorData)
            }
        }

        b_load.setOnClickListener {
            lifecycleScope.launch {
                tv_loading.text = "loading..."
                stopPlayback(engineRef)
                clear(engineRef)
                withContext(Dispatchers.IO) {
                    loadTracks(engineRef, getProjectTitle())
                }
                refreshTrackDetails(engineRef)
                tv_loading.text = null
            }
        }
        b_save.setOnClickListener {
            lifecycleScope.launch {
                tv_loading.text = "saving..."
                stopPlayback(engineRef)
                withContext(Dispatchers.IO) {
                    saveTracks(engineRef, getProjectTitle())
                }
                tv_loading.text = null
            }
        }
    }

    private fun displayCursors(cursorData: IntArray) {
        val width = loops.width
        val cursors = cursorData.asIterable().chunked(2).takeLast(loops.childCount).reversed()
        cursors.forEachIndexed { index, cursor ->
            val progress = cursor[1].toFloat() / cursor[0].toFloat() // position / size
            getTrackCursorView(index).translationX = width * progress
        }
        for (i in cursors.size until loops.childCount) {
            getTrackCursorView(i).translationX = 0f
        }
    }

    private fun refreshTrackDetails(engineRef: Long) {
        val trackDetails = getTrackDetails(engineRef)
        val details = trackDetails.asIterable()
            .chunked(1)
            .dropLast(1) // master
            .takeLast(loops.childCount)
            .reversed()

        details.forEachIndexed { index, d ->
            getTrackTitleView(index).text = d[0].toString()
        }
        for (i in details.size until loops.childCount) {
            getTrackTitleView(i).text = null
        }
    }

    private fun getTrackCursorView(index: Int): View = (loops[index] as ViewGroup)[1]
    private fun getTrackTitleView(index: Int): TextView = (loops[index] as ViewGroup)[0] as TextView
    private fun getProjectTitle(): String {
        val input = et_title_input.text.toString()
        return if (input.isNullOrBlank()) {
            "no_title"
        } else {
            input
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
    external fun deleteLoop(engineRef: Long, loopId: Int)
    external fun getLoopCursors(engineRef: Long): IntArray
    external fun getTrackDetails(engineRef: Long): IntArray
    external fun setTapeSizeMultiplier(engineRef: Long, multiplier: Double)
    external fun sampleTrack(engineRef: Long, trackId: Int, start: Int, size: Int): FloatArray
    external fun loadTrack(engineRef: Long, trackId: Int, start: Int, data: FloatArray)
    external fun clear(engineRef: Long)

    private suspend fun saveTracks(engineRef: Long, projectTitle: String) {
        val cursorData = getLoopCursors(engineRef)
        val projectDir = File(loopsDir(), "/$projectTitle")
        if (projectDir.exists()) {
            projectDir.listFiles()?.forEach { it.deleteRecursively() }
        } else {
            projectDir.mkdirs()
        }
        cursorData.asIterable().chunked(2)
            .dropLast(1) // drop tape
            .forEachIndexed { i, track ->
                Log.d("Lupe", "save track $projectTitle$i")
                saveTrack(engineRef, i, track[0], projectDir, projectTitle)
            }
    }

    private suspend fun saveTrack(
        engineRef: Long,
        trackId: Int,
        trackSize: Int,
        projectDir: File,
        projectTitle: String
    ) {
        val data: FloatArray = sampleTrack(engineRef, trackId, 0, trackSize)
        val fileName = "/$projectTitle$trackId.lp"
        val file = File(projectDir, fileName)
        if (file.exists()) {
            file.delete()
        }
        saveToFile(data, file)
    }

    private suspend fun loadTracks(engineRef: Long, projectTitle: String) {
        val loopDir = loopsDir()
        val projectDir = File(loopDir, "/$projectTitle")
        if (projectDir.exists()) {
            projectDir.listFiles()?.forEachIndexed { i, file ->
                val data = loadFromFile(file)
                loadTrack(engineRef = engineRef, trackId = i, start = 0, data = data)
            }
        }
    }

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

    private fun View.setTouchEventListener(action: Int, listener: (View) -> Unit) =
        setOnTouchListener { v, event ->
            when (event.action) {
                action -> listener(v)
            }
            false
        }
}
