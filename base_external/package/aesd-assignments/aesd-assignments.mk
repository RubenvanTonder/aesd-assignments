
##############################################################
#
# AESD-ASSIGNMENTS
#
##############################################################

#TODO: Fill up the contents below in order to reference your assignment 3 git contents
AESD_ASSIGNMENTS_VERSION = v1.2
# Note: Be sure to reference the *ssh* repository URL here (not https) to work properly
# with ssh keys and the automated build/test system.
# Your site should start with git@github.com:
AESD_ASSIGNMENTS_SITE = git@github.com:RubenvanTonder/aesd-assignments.git
AESD_ASSIGNMENTS_SITE_METHOD = git
AESD_ASSIGNMENTS_GIT_SUBMODULES = YES

define AESD_ASSIGNMENTS_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/finder-app all
endef

# TODO add your writer, finder and finder-test utilities/scripts to the installation steps below
define AESD_ASSIGNMENTS_INSTALL_TARGET_CMDS
	$(INSTALL) -d 0755 $(@D)/conf/ $(TARGET_DIR)/etc/finder/conf/
	$(INSTALL) -m 0755 $(@D)/conf/* $(TARGET_DIR)/etc/finder/conf/
	$(INSTALL) -m 0755 $(@D)/assignment-autotest/test/assignment1/assignment-test.sh $(TARGET_DIR)/usr/bin/assignment-1-test.sh
	$(INSTALL) -m 0755 $(@D)/assignment-autotest/test/assignment1/script-helpers $(TARGET_DIR)/usr/bin/

	# Install configuration files
	$(INSTALL) -d $(TARGET_DIR)/etc/finder/conf
	$(INSTALL) -m 0644 $(@D)/finder-app/conf/* $(TARGET_DIR)/etc/finder/conf/

	# Create /usr/finder-app for assignment test script relative path expectations
        $(INSTALL) -d 0755 $(TARGET_DIR)/usr/finder-app
        $(INSTALL) -d 0755 $(TARGET_DIR)/usr/bin

        # Install applications and scripts to /usr/finder-app
        $(INSTALL) -m 0755 $(@D)/finder-app/writer $(TARGET_DIR)/usr/finder-app/
	$(INSTALL) -m 0755 $(@D)/finder-app/writer.sh $(TARGET_DIR)/usr/finder-app/
        $(INSTALL) -m 0755 $(@D)/finder-app/finder.sh $(TARGET_DIR)/usr/finder-app/
        $(INSTALL) -m 0755 $(@D)/finder-app/finder-test.sh $(TARGET_DIR)/usr/finder-app/

        # Symlink them to /usr/bin for global availability
        ln -sf /usr/finder-app/writer $(TARGET_DIR)/usr/bin/writer
	ln -sf /usr/finder-app/writer.sh $(TARGET_DIR)/usr/bin/writer.sh
        ln -sf /usr/finder-app/finder.sh $(TARGET_DIR)/usr/bin/finder.sh
        ln -sf /usr/finder-app/finder-test.sh $(TARGET_DIR)/usr/bin/finder-test.sh

endef

$(eval $(generic-package))
