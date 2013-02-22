function serviceMaybeOutOfService() {
  # been out of service for a little while. To early to terminate but we should
  # scale up as a precaution

  systemCheckVariableSet OUT_OF_SERVICE_PRECAUTION_THRESHOLD || return 1
  oldest_valid_time=$(($START_TIME + ${OUT_OF_SERVICE_PRECAUTION_THRESHOLD} ))

  [ `date +%s` -gt ${oldest_valid_time} ]
  return $?
}

function serviceOutOfService() {
  # service is considered offline and will be terminated.

  systemCheckVariableSet OUT_OF_SERVICE_PRECAUTION_THRESHOLD || return 1
  oldest_valid_time=$(($START_TIME + ${OUT_OF_SERVICE_THRESHOLD} ))

  systemInfo "Service been down for $((`date +%s` - $START_TIME)) seconds."

  [ `date +%s` -gt ${oldest_valid_time} ]
  return $?
}

function serviceRetryInterval() {
  systemCheckVariableSet OUT_OF_SERVICE_PRECAUTION_THRESHOLD || return 1
  echo $(( $OUT_OF_SERVICE_PRECAUTION_THRESHOLD / 4 ))
}
