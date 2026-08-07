/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
   Copyright (c) 2017 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef HLOS_WRAPPER_API_H
#define HLOS_WRAPPER_API_H

#include <stdio.h>

/**
 * struct PositionReport
 */
typedef struct {
    /**
     * Latitude value in degrees
    */
    double mLatitude;

    /**
     * Longitude value in degrees
    */
    double mLongitude;

    /**
     * Horizontal accuracy is meters reported with confidence as
     * specified by mHorizontalConfidence.
    */
    float mHorzizontalUncertainity;

    /**
     * Altitude in meteres wrt ellipsoid. Optional field.
     */
    float mAltitudeWrtEllipsoid;

    /**
     * Vertical accuracy is meters reported with confidence as
     * specified by mVerticalConfidence. Optional field.
     */
    float mVerticalUncertainity;

    /**
     * Confidence of the horizontal accuracy reported
    */
    uint mHorizontalConfidence;

    /**
     * Confidence of the vertical accuracy reported.
     * Optional field.
    */
    uint mVerticalConfidence;

    /**
     * UTC Timestamp in milliseconds of when the fix was generated.
    */
    long long mUtcTimestamp;

    /**
     * Boolean value to indicate whether altitude is valid.
    */
    bool mHasAltitudeWrtEllipsoid;

    /**
     * Boolean value to indicate whether vertical uncertainity and
     * vertical confidence is valid.
    */
    bool mHasVerticalUncertainity;
} PositionReport;

/**
 * struct PositionError
 */
typedef enum {
    /**
     * Value to indicate no valid position could be generated.
    */
    PositionError_NoPosition,

} PositionError;

typedef void (*POSITION_CB) (PositionReport pos);
typedef void (*ERROR_CB) (PositionError err);

/**
 * @brief API to do some initialization work
 * @return void
 */
void init();

/**
 * @brief API to do some de-initialization work.
 * If any callback functions were registered using registerCallbacks API, they
 * will be set to NULL internally.
 * @return void
 */
 void deinit();

/**
 * @brief API to register for position and error callbacks
 * @details Callbacks must be registered before requesting for positon fix.
 * @return void
 */
void registerCallbacks(POSITION_CB posCb, ERROR_CB errCb);


/**
 * @brief API to request for GTP WWAN fix.
 * @param isHighPriority Data limits are overriden if request is high priority.
 * @return void
 */
void requestGtpWwanFix(bool isHighPriority);

/**
 * @brief API to enable and disable WWAN by sending user opt-in preference.
 * @param userPreference Boolean value to indicate user opt-in preference.
 * @return void
 */
void setOptIn(bool userPreference);

#endif /* HLOS_WRAPPER_API_H */
