#ifndef SRC_COORDINATE_ENUMS
#define SRC_COORDINATE_ENUMS

enum class CoordinateComponent {
  Latitude,
  Longitude,
};

enum class CoordinatePart {
  Degrees,
  Minutes,
  Seconds,
};

enum class CoordinateFormat {
  Degrees,
  DegreesWithHemisphere,
  DegreesMinutes,
  DegreesMinutesWithHemisphere,
  DegreesMinutesSeconds,
  DegreesMinutesSecondsWithHemisphere,
};

#endif // SRC_COORDINATE_ENUMS
