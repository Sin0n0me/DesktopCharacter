#pragma once

struct alignas(16) AlphaMaskParams {
    unsigned int width;
    unsigned int height;
    float alpha_threshold;
};
