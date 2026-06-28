/**
 * vplayx: A browser extension that automaticaly plays your videos
 * comments: this extension is personalized so if you want
 *           to use it on other sites, you should go to the manifest
 *           and change the url on line 11 to the url you want it
 *           to work with.
 * to: whoever wants to use this addon.
 * by: whoever wrote it.
 */

console.log("[vplayx] Extension loaded");

let currentVideo = null;
let lastUrl = location.href;

function tryPlay(video) {
    if (!video) return;

    console.log("[vplayx] Trying to play...", video);

    video.play()
        .then(() => console.log("[vplayx] Playing"))
        .catch(err => console.log("[vplayx] play() failed:", err));
}

function hookVideo(video) {
    if (video === currentVideo) return;

    currentVideo = video;

    console.log("[vplayx] New video detected");

    // Try immediately
    tryPlay(video);

    // Try when metadata arrives
    video.addEventListener("loadedmetadata", () => {
        console.log("[vplayx] loadedmetadata");
        tryPlay(video);
    });

    // Try when it can play
    video.addEventListener("canplay", () => {
        console.log("[vplayx] canplay");
        tryPlay(video);
    });

    // Keep trying for 10 seconds because React players
    // often replace the element after creation.
    let attempts = 0;

    const interval = setInterval(() => {
        attempts++;

        if (video.paused) {
            console.log("[vplayx] Retry", attempts);
            tryPlay(video);
        } else {
            console.log("[vplayx] Success");
            clearInterval(interval);
        }

        if (attempts >= 20) {
            clearInterval(interval);
        }
    }, 500);
}

function scan() {
    const video = document.querySelector("video");

    if (video) {
        hookVideo(video);
    }
}

const observer = new MutationObserver(() => {
    scan();

    if (location.href !== lastUrl) {
        lastUrl = location.href;

        console.log("[vplayx] URL changed");

        setTimeout(scan, 500);
        setTimeout(scan, 1000);
        setTimeout(scan, 2000);
    }
});

observer.observe(document.documentElement, {
    childList: true,
    subtree: true
});

scan();
