package com.darrell2604.opencrossing

import android.app.Activity
import android.content.Intent
import android.net.Uri
import android.os.Bundle
import android.view.View
import android.view.Window
import android.view.WindowManager
import android.widget.Toast

class MainActivity : Activity() {
    private lateinit var gameView: NativeGameView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        requestWindowFeature(Window.FEATURE_NO_TITLE)
        window.setFlags(
            WindowManager.LayoutParams.FLAG_FULLSCREEN,
            WindowManager.LayoutParams.FLAG_FULLSCREEN
        )
        window.decorView.systemUiVisibility = (
            View.SYSTEM_UI_FLAG_FULLSCREEN or
            View.SYSTEM_UI_FLAG_HIDE_NAVIGATION or
            View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY or
            View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN or
            View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION or
            View.SYSTEM_UI_FLAG_LAYOUT_STABLE
        )

        gameView = NativeGameView(this)
        setContentView(gameView)

        if (savedInstanceState == null) {
            openDiscPicker()
        }
    }

    private fun openDiscPicker() {
        val intent = Intent(Intent.ACTION_OPEN_DOCUMENT).apply {
            addCategory(Intent.CATEGORY_OPENABLE)
            type = "application/octet-stream"
        }
        startActivityForResult(intent, REQUEST_DISC)
    }

    @Deprecated("Use Activity Result APIs when this prototype grows a UI")
    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (requestCode != REQUEST_DISC || resultCode != RESULT_OK) return
        val uri: Uri = data?.data ?: return
        try {
            contentResolver.openInputStream(uri).use { input ->
                requireNotNull(input) { "Unable to open selected disc image" }
                val bytes = input.readBytes()
                val ok = gameView.loadDisc(bytes)
                val message = if (ok) "Animal Crossing disc loaded" else "Disc image rejected"
                Toast.makeText(this, message, Toast.LENGTH_SHORT).show()
            }
        } catch (e: Exception) {
            Toast.makeText(this, "Disc load failed: ${e.message}", Toast.LENGTH_LONG).show()
        }
    }

    override fun onPause() {
        gameView.onPause()
        super.onPause()
    }

    override fun onResume() {
        super.onResume()
        gameView.onResume()
    }

    companion object {
        private const val REQUEST_DISC = 1001

        init {
            System.loadLibrary("opencrossing")
        }
    }
}
