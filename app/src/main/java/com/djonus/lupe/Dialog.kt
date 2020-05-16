package com.djonus.lupe

import android.content.Context
import android.widget.EditText
import androidx.appcompat.app.AlertDialog

fun Context.dialogStringInput(
    title: String,
    hint: String,
    initialInputValue: String,
    ok: (String) -> Boolean
) {
    val input = EditText(this).apply {
        setHint(hint)
        setText(initialInputValue)
    }
    AlertDialog.Builder(this)
        .setCancelable(true)
        .setTitle(title)
        .setView(input)
        .setPositiveButton("OK") { d, i ->
            if (ok(input.text.toString())) {
                d.dismiss()
            }
        }
        .show()
}
