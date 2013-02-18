function serviceGetTimeLastHealthy() {
  cat ${SERVICE_FILE} 2> /dev/null || echo 0
}

function serviceUpdateTimeLastHealthy() {
  date +%s > ${SERVICE_FILE}
}
