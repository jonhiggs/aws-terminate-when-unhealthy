function serviceGetTimeLastHealthy() {
  cat ${SERVICE_FILE} 2> /dev/null || echo 0
}

function serviceUpdateTimeLastHealthy() {
  date +%s > ${SERVICE_FILE}
}

function serviceOutOfService() {
  # service is considered offline and will be terminated.
  oldest_valid_time=$((`date +%s` - ${OUT_OF_SERVICE_THRESHOLD} ))

  if [ `serviceGetTimeLastHealthy` -eq 0 ]; then
    return 1
  elif [ `serviceGetTimeLastHealthy` -lt ${oldest_valid_time} ]; then
    return 0
  else
    return 1
  fi
}
