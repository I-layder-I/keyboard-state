#include "evdevreader.h"
#include <QDebug>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

EvdevReader::EvdevReader(const QString &path, QObject *parent)
    : QObject(parent), path_(path) {
  fd_ = ::open(path.toUtf8().constData(), O_RDONLY | O_NONBLOCK);
  if (fd_ < 0) {
    qWarning() << "Не удалось открыть" << path << ":" << std::strerror(errno);
    return;
  }
  notifier_ = new QSocketNotifier(fd_, QSocketNotifier::Read, this);
  connect(notifier_, &QSocketNotifier::activated, this,
          &EvdevReader::onReadyRead);
}

EvdevReader::~EvdevReader() {
  if (fd_ >= 0)
    ::close(fd_);
}

void EvdevReader::onReadyRead() {
  struct input_event ev[64];
  ssize_t sz = ::read(fd_, ev, sizeof(ev));
  if (sz <= 0)
    return;
  int n = int(sz / sizeof(struct input_event));
  for (int i = 0; i < n; ++i) {
    if (ev[i].type == EV_KEY)
      emit eventReceived(ev[i]);
  }
}
