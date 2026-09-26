# Changelog

All notable changes to WetDelay are recorded here. The published notes for each
release are on the [Releases page](https://github.com/yonie/WetDelay/releases);
this file is the portable copy that travels with the source.

Format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and
this project uses [semantic versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [1.3.0] - 2026-09-26

### Added
- An Audio Unit for Logic and GarageBand, next to the VST3.
- Mono tracks: all four layouts (mono or stereo in, mono or stereo out). A mono
  input feeds both sides; a mono output is the average of left and right.

### Changed
- The macOS builds are signed and notarised by Apple.
- The sound is unchanged, and saved projects reload with the same settings.

## [1.2.1] - 2026-08-30

### Fixed
- Level meters read far lower than the signal actually was. They mapped
  amplitude linearly across their segments, so half the strip covered -6 dBFS
  to 0: a -20 dBFS signal lit one segment of twelve and anything below -24 dBFS
  lit none. They now read in dB, one segment per equal step from -48 to +18.

## [1.2.0] - 2026-08-28

### Added
- UI Zoom for high-resolution displays - right-click the panel and pick 75%,
  100% or 125%.

## [1.1.5] - 2026-08-26

### Fixed
- Intermittent clicks that got worse the longer the plugin was left running and
  were worst at small buffer sizes. The plugin could also occasionally overshoot
  and distort. The sound is otherwise unchanged.

## [1.1.4] - 2026-04-17

## [1.1.3] - 2026-03-28

### Fixed
- The selected delay time was not remembered across DAW sessions.

## [1.1.2] - 2026-03-28

### Added
- Companding, bringing the audible noise floor down to -78 dBFS effective.

## [1.1.1] - 2026-03-30

## [1.1.0] - 2026-03-09

### Added
- Universal VST3 bundle covering Windows x64, Linux x86_64 and macOS on both
  Intel and Apple Silicon, in one download.
- GitHub Actions CI for automated cross-platform builds.

## [1.0.0] - 2026-01-07

Initial release.

### Added
- Stereo delay, 100% wet, with six fixed delay times from 20 to 400 ms.
- Input and output peak metering.
- Full VST3 parameter automation.
- 80s rack character: 24 kHz internal sample rate with resampling, 12-bit
  quantisation, TPDF dither, and anti-alias and reconstruction filters.
- -40 dB (1%) L/R channel bleed.
