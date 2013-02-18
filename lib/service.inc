function serviceTimeLastHealthy() {
  cat ${SERVICE_FILE} 2> /dev/null || echo 0
}
