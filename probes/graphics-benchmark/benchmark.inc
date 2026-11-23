#define WARMUP_FRAMES 60
#define SAMPLE_FRAMES 180

typedef struct {
    unsigned width;
    unsigned height;
    unsigned rows;
    unsigned vertices;
} BenchmarkProfile;

typedef struct {
    double averageFps;
    double p95Ms;
    unsigned slowFrames;
} BenchmarkResult;

static const BenchmarkProfile profiles[] = {
    { 854, 480, 12, 3 },
    { 640, 360, 12, 3 },
    { 854, 480, 12, 6 },
    { 854, 480, 36, 6 },
};

static int compareTicks(const void* left, const void* right)
{
    const u64 a = *(const u64*)left;
    const u64 b = *(const u64*)right;
    return (a > b) - (a < b);
}

static bool measure(const BenchmarkProfile* profile, PadState* pad, BenchmarkResult* result)
{
    graphicsInitialize(profile->width, profile->height, profile->rows, profile->vertices);

    u64 samples[SAMPLE_FRAMES];
    u64 totalTicks = 0;
    unsigned slowFrames = 0;
    unsigned sampleCount = 0;
    const u64 slowThreshold = armNsToTicks(34000000);
    u64 previousTick = armGetSystemTick();

    for (unsigned frame = 0; frame < WARMUP_FRAMES + SAMPLE_FRAMES && appletMainLoop(); ++frame)
    {
        padUpdate(pad);
        if (padGetButtonsDown(pad) & HidNpadButton_Plus)
            break;

        graphicsUpdate();
        const u64 now = armGetSystemTick();
        const u64 elapsed = now - previousTick;
        previousTick = now;

        if (frame >= WARMUP_FRAMES)
        {
            samples[sampleCount++] = elapsed;
            totalTicks += elapsed;
            slowFrames += elapsed > slowThreshold;
        }
    }

    graphicsExit();

    if (sampleCount != SAMPLE_FRAMES || totalTicks == 0)
        return false;

    qsort(samples, SAMPLE_FRAMES, sizeof(samples[0]), compareTicks);
    const unsigned p95Index = (95 * SAMPLE_FRAMES + 99) / 100 - 1;
    result->averageFps = (double)SAMPLE_FRAMES * armGetSystemTickFreq() / totalTicks;
    result->p95Ms = armTicksToNs(samples[p95Index]) / 1000000.0;
    result->slowFrames = slowFrames;
    return true;
}

int main(void)
{
    if (R_FAILED(romfsInit()))
        return 1;

    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);

    enum { profileCount = sizeof(profiles) / sizeof(profiles[0]) };
    BenchmarkResult results[profileCount];
    unsigned completed = 0;
    while (completed < profileCount && measure(&profiles[completed], &pad, &results[completed]))
        ++completed;

    romfsExit();
    consoleInit(NULL);
    printf("Switch Lite graphics benchmark | 30 FPS target\n\n");
    printf("Internal   Rows  Tris  Avg FPS   p95 ms   >34 ms\n");
    for (unsigned i = 0; i < completed; ++i)
    {
        const BenchmarkProfile* p = &profiles[i];
        const BenchmarkResult* r = &results[i];
        printf("%3ux%-3u    %2u   %2u    %6.2f   %6.2f   %3u/%u\n",
            p->width, p->height, p->rows, p->vertices / 3,
            r->averageFps, r->p95Ms, r->slowFrames, SAMPLE_FRAMES);
    }
    printf("\nCompleted %u/%u profiles. Press PLUS to exit.\n", completed, profileCount);
    while (appletMainLoop())
    {
        padUpdate(&pad);
        if (padGetButtonsDown(&pad) & HidNpadButton_Plus)
            break;
        consoleUpdate(NULL);
    }
    consoleExit(NULL);
    return 0;
}
