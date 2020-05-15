package com.djonus.lupe.utils

import android.content.Context
import android.util.Log
import java.io.File
import java.io.FileInputStream
import java.io.FileOutputStream
import java.nio.ByteBuffer

suspend fun saveToFile(data: FloatArray, file: File) {
    try {
        val out = FileOutputStream(file)
        data.forEachIndexed { index, sample ->
            Log.d("LUPE", "File saving: ${index.toFloat() / data.size}%")
            val bytes = ByteBuffer.allocate(4).putFloat(sample).array()
            out.write(bytes)
        }
        out.flush()
        out.close()
        Log.d("LUPE", "File saved: $file")
    } catch (e: Exception) {
        e.printStackTrace()
        Log.d("LUPE", "File save failed: $file")
    }
}

suspend fun loadFromFile(file: File): FloatArray {
    val data = mutableListOf<Float>()
    try {
        val ins = FileInputStream(file)
        val buffer = ByteArray(4)

        var byte = ins.read(buffer)
        while (byte != -1) {
            Log.d("LUPE", "Loading loop: ${data.size}")
            data.add(ByteBuffer.wrap(buffer).float)
            byte = ins.read(buffer)
        }
        Log.d("LUPE", "File loaded: ${data.size}, $file")
        ins.close()
    } catch (e: Exception) {
        e.printStackTrace()
        Log.d("LUPE", "File load error: $file")
    }
    return data.toFloatArray()
}

fun Context.loopsDir(): File {
    val root = getExternalFilesDir(null)
    return File(root, "/LupeLoops")
}
