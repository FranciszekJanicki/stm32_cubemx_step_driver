FROM archlinux:base-devel

# Install base dependencies and necessary tools
RUN pacman -Sy --noconfirm --needed \
    base-devel \
    arm-none-eabi-gcc \
    arm-none-eabi-newlib \
    curl \
    unzip \
    jre-openjdk \
    qt5-base \
    libusb \
    usbutils \
    cmake \
    make \
    git \
    minicom \
    stlink \
    sudo \
    && pacman -Scc --noconfirm

# Create a new user for building
RUN useradd -m builder && echo "builder ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/builder

USER builder

# Install yay for AUR support and build it
RUN git clone https://aur.archlinux.org/yay-bin.git /tmp/yay-bin && \
    cd /tmp/yay-bin && makepkg -si --noconfirm && \
    rm -rf /tmp/yay-bin

# Update yay and install STM32CubeMX and STM32CubeProgrammer
RUN yay -Syu --noconfirm && \
    yay -S --noconfirm --needed stm32cubemx 

# Switch to root for modifying USB rules
USER root

# # Add udev rule for STM32 devices (ST-Link/STM32 programmers)
# RUN echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="0483", ATTR{idProduct}=="3748", MODE="0666", GROUP="plugdev"' > /etc/udev/rules.d/99-stlink.rules && \
#     udevadm control --reload-rules && \
#     udevadm trigger

# Switch to bash shell
SHELL ["/bin/bash", "-c"]

