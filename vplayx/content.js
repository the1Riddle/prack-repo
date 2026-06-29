/**
 * vplayx: A browser extension that automatically plays your videos
 * comments: this extension is personalized, so if you want
 *           To use it on other sites, you should go to the manifest
 *           and change the URL on line 11 to the URL you want it
 *           to work with.
 * To: whoever wants to use this addon.
 * by: whoever wrote it.
 */

console.log("[vplayx] Extension loaded");

let currentVideo = null;
let lastUrl = location.href;

// ------------------------------------------------------
// Play video
// ------------------------------------------------------

function tryPlay(video) {
    if (!video) return;

    console.log("[vplayx] Trying to play...");

    video.play()
        .then(() => {
            console.log("[vplayx] Playing");
        })
        .catch(err => {
            console.log("[vplayx] play() failed:", err.name);
        });
}

// ------------------------------------------------------
// Go to next topic
// ------------------------------------------------------

function goToNextTopic() {
    const nextButton = document.querySelector(
        'button[aria-label="Next Topic"]'
    );

    if (!nextButton) {
        console.log("[vplayx] Course complete?");
        return;
    }

    if (nextButton.disabled || nextButton.ariaDisabled === "true") {
        console.log("[vplayx] Last lesson reached");
        return;
    }

    nextButton.click();
}

// ------------------------------------------------------
// Hook newly created videos
// ------------------------------------------------------

function hookVideo(video) {
    if (video.dataset.vplayxHooked) {
        return;
    }

    video.dataset.vplayxHooked = "true";

    currentVideo = video;

    console.log("[vplayx] New video detected");

    // Auto play
    tryPlay(video);

    video.addEventListener("loadedmetadata", () => {
        console.log("[vplayx] loadedmetadata");
        tryPlay(video);
    });

    video.addEventListener("canplay", () => {
        console.log("[vplayx] canplay");
        tryPlay(video);
    });

    // Retry for a few seconds
    let attempts = 0;

    const retryTimer = setInterval(() => {
        attempts++;

        if (!video.paused) {
            clearInterval(retryTimer);
            return;
        }

        if (attempts >= 20) {
            clearInterval(retryTimer);
            return;
        }

        console.log(`[vplayx] Retry ${attempts}`);

        tryPlay(video);
    }, 500);

    //----------------------------------------------------
    // Auto advance
    //----------------------------------------------------

    let finished = false;

    function handleFinished() {
        if (finished)
            return;

        finished = true;

        console.log("[vplayx] Video finished");

        setTimeout(() => {
            // Site already moved?
            if (currentVideo !== video)
                return;

            goToNextTopic();
        }, 2000);
    }

    video.addEventListener("ended", handleFinished);

    // Just incase "ended" is never fired
    video.addEventListener("timeupdate", () => {
        if (finished)
            return;

        if (
            video.duration &&
            video.currentTime >= video.duration - 0.5
        ) {
            handleFinished();
        }
    });
}

// ------------------------------------------------------
// Scan page
// ------------------------------------------------------

function scanForVideos() {

    const videos = document.querySelectorAll("video");

    videos.forEach(hookVideo);

}

// ------------------------------------------------------
// Observe DOM changes
// ------------------------------------------------------

const observer = new MutationObserver(() => {
    scanForVideos();

    if (location.href !== lastUrl) {
        lastUrl = location.href;

        console.log("[vplayx] URL changed");

        setTimeout(scanForVideos, 500);
        setTimeout(scanForVideos, 1000);
        setTimeout(scanForVideos, 2000);
    }
});

observer.observe(document.documentElement, {
    childList: true,
    subtree: true
});

scanForVideos();
