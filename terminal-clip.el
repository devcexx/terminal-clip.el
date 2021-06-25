;;; terminal-clip.el --- Clipboard support on terminal.
;;; Commentary:

;; This file defines a global minor mode `terminal-clip-mode' that
;; allows interacting with the window manager clipboard when running
;; Emacs in headless mode.  This mode is not intended to define new
;; logics for kill and yank commands, nor new keybindings for
;; interacting with the clipboard in a special way.  Instead, the idea
;; behind it is to port the clipboard functionality that Emacs does by
;; default when running on a windowed environment to a terminal
;; environment.

;;; Code:

(defun terminal-clip--get-unavailability-reason ()
  "Verify that terminal-clip is supported under the current conditions."
  (cond ((not (and (symbolp 'module-file-suffix) module-file-suffix)) 'no-module-support)
	(window-system 'window-system)
	(t nil)))


(defconst terminal-clip-unavailability-reason (terminal-clip--get-unavailability-reason)
  "Contain a value that indicate the reason why the terminal-clip mode is not currently available, or nil otherwise.")
(defconst terminal-clip-available (eq terminal-clip-unavailability-reason nil)
  "Indicate whether the terminal-clip mode is currently available.")


(defun terminal-clip--get-selection-handler (selection target)
  "Override for the `gui-backend-get-selection' that return the current text of the window system clipboard, when SELECTION and TARGET are set to 'CLIPBOARD and 'STRING, respectively."
  (when (and (eq selection 'CLIPBOARD) (eq target 'STRING))
    (terminal-clip-read-clipboard-text)))


(defun terminal-clip--set-selection-handler (selection value)
  "Override for the `gui-backend-set-selection' that set the current text of the window system clipboard to VALUE, only when SELECTION is 'CLIPBOARD."
  (when (eq selection 'CLIPBOARD)
    (terminal-clip-write-clipboard-text value)))


(defun terminal-clip--enable ()
  "Enable the terminal-clip mode."
  (pcase terminal-clip-available
    ('window-system (user-error "Terminal clip not available while running with window system support (kinda obvious right?)"))
    ('no-module-support (user-error "This Emacs version doesn't seem to have dynamic module loading support.  Was it compiled with the --with-modules flag?"))
    ('t )
    (err (error "Unexpected result from availability verification %S.  This must not happen!" err)))

  (require 'terminal-clip-native)

  (advice-add
   'gui-backend-get-selection
   :override #'terminal-clip--get-selection-handler)

  (advice-add
   'gui-backend-set-selection
   :override #'terminal-clip--set-selection-handler))


(defun terminal-clip--disable ()
  "Disable the terminal-clip mode."
  (advice-remove 'gui-backend-get-selection #'terminal-clip--get-selection-handler)
  (advice-remove 'gui-backend-set-selection #'terminal-clip--set-selection-handler))


(define-minor-mode terminal-clip-mode
  "Terminal clip minor mode."
  :init-value nil
  :global t
  :group 'mode

  (if terminal-clip-mode
      (terminal-clip--enable)
    (terminal-clip--disable)))

(provide 'terminal-clip)

