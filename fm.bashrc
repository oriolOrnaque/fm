export FM_LASTD='/tmp/fm_lastd'

fm () {
	~/fm/a.out && cd $(cat $FM_LASTD)
}

