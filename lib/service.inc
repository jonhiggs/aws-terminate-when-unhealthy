function serviceGetTimeLastHealthy() {
  systemCheckVariableSet SERVICE_FILE || return 1
  if [ -s ${SERVICE_FILE} ]; then
    cat ${SERVICE_FILE} 2> /dev/null
  else
    echo 0
  fi
}

function serviceUpdateTimeLastHealthy() {
  systemCheckVariableSet SERVICE_FILE || return 1
  date +%s > ${SERVICE_FILE}
}

function serviceMaybeOutOfService() {
  # been out of service for a little while. To early to terminate but we should
  # scale up as a precaution

  systemCheckVariableSet SERVICE_FILE || return 1
  systemCheckVariableSet OUT_OF_SERVICE_PRECAUTION_THRESHOLD || return 1

  oldest_valid_time=$((`date +%s` - ${OUT_OF_SERVICE_PRECAUTION_THRESHOLD} ))
  if [ ! -s ${SERVICE_FILE} ]; then return 1; fi
  if [ `serviceGetTimeLastHealthy` -gt $oldest_valid_time ]; then return 1; fi
}

function serviceOutOfService() {
  # service is considered offline and will be terminated.

  systemCheckVariableSet SERVICE_FILE || return 1
  systemCheckVariableSet OUT_OF_SERVICE_PRECAUTION_THRESHOLD || return 1

  oldest_valid_time=$((`date +%s` - ${OUT_OF_SERVICE_THRESHOLD} ))
  if [ ! -s ${SERVICE_FILE} ]; then return 1; fi
  if [ `serviceGetTimeLastHealthy` -gt ${oldest_valid_time} ]; then return 1; fi
}
