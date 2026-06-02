/**
 * vplayx: A browser extension that automaticaly plays your videos
 * comments: this extension is personalized so if you want
 *           to use it on other sites, you should go to the manifest
 *           and change the url on line 11 to the url you want it
 *           to work with.
 * to: whoever wants to use this addon.
 * by: whoever wrote it.
 */

let lastUrl = location.href;

function autoPlayVideo() {
    const video = document.querySelector('video');

    if (video && video.paused) {
        video.play().catch(() => {});
        return;
    }

    // Fallback to clicking play button
    const playButton = document.querySelector('.play-button-class');

    if (playButton) {
        playButton.click();
    }
}

const observer = new MutationObserver(() => {
    autoPlayVideo();

    if (location.href !== lastUrl) {
        lastUrl = location.href;

        setTimeout(autoPlayVideo, 1000);
        setTimeout(autoPlayVideo, 3000);
    }
});

observer.observe(document.documentElement, {
    childList: true,
    subtree: true
});

window.addEventListener('load', autoPlayVideo);
autoPlayVideo();
