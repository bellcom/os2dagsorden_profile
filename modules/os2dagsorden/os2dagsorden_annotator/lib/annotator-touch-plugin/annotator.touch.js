/*  Annotator Touch Plugin - v2.1.2
 *  Copyright 2012-2020, Compendio <www.compendio.ch>
 *  Released under the MIT license
 *  More Information: https://github.com/aron/annotator.touch.js
 */
if (!window.annotator.touch) window.annotator.touch = {};
window.annotator.touch.adder = {};

(function() {
  var Widget = window.annotator.ui.widget.Widget,
    util = window.annotator.util;

  var $ = util.$;
  var _t = util.gettext;

  var NS = "annotator-adder";

  // Adder shows and hides an annotation adder button that can be clicked on to
  // create an annotation.
  var Adder = Widget.extend({
    constructor: function(options) {
      Widget.call(this, options);

      this.annotation = null;
      this.isVisible = false;
      this.onCreate = this.options.onCreate;

      var self = this;
      this.element.on("tap." + NS, "a.annotator-button", function(e) {
        self._onClick(e);
      });
    },

    destroy: function() {
      this.element.off("." + NS);
      Widget.prototype.destroy.call(this);
    },

    // Public: Load an annotation and show the adder.
    //
    // annotation - An annotation Object to load.
    //
    // If the user clicks on the adder with an annotation loaded, the onCreate
    // handler will be called. In this way, the adder can serve as an
    // intermediary step between making a selection and creating an annotation.
    //
    // Returns nothing.
    load: function(annotation) {
      this.annotation = annotation;
      this.show();
    },

    // Public: Show the adder.
    //
    // Examples
    //
    //   adder.show()
    //   adder.hide()
    //
    // Returns nothing.
    show: function() {
      if (!this.isVisible) {
        if (window.annotator.touch.enabler.instance) {
          window.annotator.touch.enabler.instance.hide();
        }
        Widget.prototype.show.call(this);
        this.isVisible = true;
      }
    },
    hide: function() {
      if (this.isVisible) {
        if (window.annotator.touch.enabler.instance) {
          window.annotator.touch.enabler.instance.show();
        }
        Widget.prototype.hide.call(this);
        this.isVisible = false;
      }
    },

    // Event callback: called when the adder is clicked.
    //
    // event - A tap Event object
    //
    // Returns nothing.
    _onClick: function(event) {
      event.preventDefault();

      // Hide the adder
      this.hide();

      // Create a new annotation
      if (this.annotation !== null && typeof this.onCreate === "function") {
        this.onCreate(this.annotation, event);

        if (window.annotator.touch.enabler.instance) {
          window.annotator.touch.enabler.instance.disable();
        }
      }
    }
  });

  Adder.classes = {
    hide: "annotator-touch-hide"
  };

  Adder.template = [
    '<div class="annotator-touch-widget annotator-touch-controls annotator-touch-hide">',
    '  <div class="annotator-touch-widget-inner">',
    '    <a class="annotator-button annotator-add annotator-focus">' +
      _t("Annotate") +
      "</a>",
    "  </div>",
    "</div>"
  ].join("\n");

  // Configuration options
  Adder.options = {
    // Callback, called when the user clicks the adder when an
    // annotation is loaded.
    onCreate: null
  };

  window.annotator.touch.adder.Adder = Adder;
})();
if (!window.annotator.touch) window.annotator.touch = {};
window.annotator.touch.enabler = {};

(function() {
  var Widget = window.annotator.ui.widget.Widget,
    util = window.annotator.util;

  var $ = util.$;
  var _t = util.gettext;

  var NS = "annotator-enabler";

  var Enabler = Widget.extend({
    constructor: function() {
      Widget.call(this, {});

      var self = this;
      self.enabled = false;
      self.callbacks = [];
      this.element.on("tap." + NS, "a.annotator-button", function(e) {
        self._onClick(e);
      });
    },

    destroy: function() {
      this.element.off("." + NS);
      Widget.prototype.destroy.call(this);
    },

    disable: function() {
      this.enabled = false;

      this._update();
    },

    isEnabled: function() {
      return this.enabled;
    },

    onChange: function(callback) {
      this.callbacks.push(callback);
    },

    _update: function(event) {
      this.element.find(".annotator-enable").text(this.enabled ? _t("Disable annotation") : _t("Enable annotation"));

      // Run callbacks
      $.each(this.callbacks, function () {
        this();
      });
    },

    _onClick: function(event) {
      event.preventDefault();

      this.enabled = !this.enabled;

      this._update();
    }
  });

  Enabler.classes = {
    hide: "annotator-touch-hide"
  };

  Enabler.template = [
    '<div class="annotator-touch-widget annotator-touch-controls annotator-touch-hide">',
    '  <div class="annotator-touch-widget-inner">',
    '    <a class="annotator-button annotator-enable annotator-focus">' +
      _t("Enable annotation") +
      "</a>",
    "  </div>",
    "</div>"
  ].join("\n");

  window.annotator.touch.enabler.Enabler = Enabler;
})();
if (!window.annotator.touch) window.annotator.touch = {};
window.annotator.touch.editor = {};

(function() {
  var Widget = window.annotator.ui.widget.Widget,
    util = window.annotator.util;

  var $ = util.$;
  var _t = util.gettext;
  var Promise = util.Promise;

  var NS = "annotator-editor";

  // id returns an identifier unique within this session
  var id = (function() {
    var counter;
    counter = -1;
    return function() {
      return (counter += 1);
    };
  })();

  // preventEventDefault prevents an event's default, but handles the condition
  // that the event is null or doesn't have a preventDefault function.
  function preventEventDefault(event) {
    if (
      typeof event !== "undefined" &&
      event !== null &&
      typeof event.preventDefault === "function"
    ) {
      event.preventDefault();
    }
  }

  // Public: Creates an element for editing annotations.
  var Editor = (window.annotator.touch.editor.Editor = Widget.extend({
    // Public: Creates an instance of the Editor object.
    //
    // options - An Object literal containing options.
    //
    // Examples
    //
    //   # Creates a new editor, adds a custom field and
    //   # loads an annotation for editing.
    //   editor = new Annotator.Editor
    //   editor.addField({
    //     label: 'My custom input field',
    //     type:  'textarea'
    //     load:  someLoadCallback
    //     save:  someSaveCallback
    //   })
    //   editor.load(annotation)
    //
    // Returns a new Editor instance.
    constructor: function(options) {
      Widget.call(this, options);

      this.fields = [];
      this.annotation = {};

      if (this.options.defaultFields) {
        this.addField({
          type: "textarea",
          label: _t("Comments") + "\u2026",
          load: function(field, annotation) {
            $(field)
              .find("textarea")
              .val(annotation.text || "");
          },
          submit: function(field, annotation) {
            annotation.text = $(field)
              .find("textarea")
              .val();
          }
        });
      }

      var self = this;

      this.element
        .on("submit." + NS, "form", function(e) {
          self._onFormSubmit(e);
        })
        .on("tap." + NS, ".annotator-save", function(e) {
          self._onSaveClick(e);
        })
        .on("tap." + NS, ".annotator-cancel", function(e) {
          self._onCancelClick(e);
        });
    },

    destroy: function() {
      this.element.off("." + NS);
      Widget.prototype.destroy.call(this);
    },

    // Public: Show the editor.
    //
    // Examples
    //
    //   editor.show()
    //   editor.hide()
    //
    // Returns nothing.
    show: function() {
      this.element.find(".annotator-save").addClass(this.classes.focus);

      Widget.prototype.show.call(this);

      // give main textarea focus
      this.element.find(":input:first").focus();
    },

    // Public: Load an annotation into the editor and display it.
    //
    // annotation - An annotation Object to display for editing.
    //
    // Returns a Promise that is resolved when the editor is submitted, or
    // rejected if editing is cancelled.
    load: function(annotation) {
      this.annotation = annotation;

      for (var i = 0, len = this.fields.length; i < len; i++) {
        var field = this.fields[i];
        field.load(field.element, this.annotation);
      }

      var self = this;
      return new Promise(function(resolve, reject) {
        self.dfd = { resolve: resolve, reject: reject };
        self.show();
      });
    },

    // Public: Submits the editor and saves any changes made to the annotation.
    //
    // Returns nothing.
    submit: function() {
      for (var i = 0, len = this.fields.length; i < len; i++) {
        var field = this.fields[i];
        field.submit(field.element, this.annotation);
      }
      if (typeof this.dfd !== "undefined" && this.dfd !== null) {
        this.dfd.resolve();
      }
      this.hide();
    },

    // Public: Cancels the editing process, discarding any edits made to the
    // annotation.
    //
    // Returns itself.
    cancel: function() {
      if (typeof this.dfd !== "undefined" && this.dfd !== null) {
        this.dfd.reject("editing cancelled");
      }
      this.hide();
    },

    // Public: Adds an addional form field to the editor. Callbacks can be
    // provided to update the view and anotations on load and submission.
    //
    // options - An options Object. Options are as follows:
    //           id     - A unique id for the form element will also be set as
    //                    the "for" attrubute of a label if there is one.
    //                    (default: "annotator-field-{number}")
    //           type   - Input type String. One of "input", "textarea",
    //                    "checkbox", "select" (default: "input")
    //           label  - Label to display either in a label Element or as
    //                    placeholder text depending on the type. (default: "")
    //           load   - Callback Function called when the editor is loaded
    //                    with a new annotation. Receives the field <li> element
    //                    and the annotation to be loaded.
    //           submit - Callback Function called when the editor is submitted.
    //                    Receives the field <li> element and the annotation to
    //                    be updated.
    //
    // Examples
    //
    //   # Add a new input element.
    //   editor.addField({
    //     label: "Tags",
    //
    //     # This is called when the editor is loaded use it to update your
    //     # input.
    //     load: (field, annotation) ->
    //       # Do something with the annotation.
    //       value = getTagString(annotation.tags)
    //       $(field).find('input').val(value)
    //
    //     # This is called when the editor is submitted use it to retrieve data
    //     # from your input and save it to the annotation.
    //     submit: (field, annotation) ->
    //       value = $(field).find('input').val()
    //       annotation.tags = getTagsFromString(value)
    //   })
    //
    //   # Add a new checkbox element.
    //   editor.addField({
    //     type: 'checkbox',
    //     id: 'annotator-field-my-checkbox',
    //     label: 'Allow anyone to see this annotation',
    //     load: (field, annotation) ->
    //       # Check what state of input should be.
    //       if checked
    //         $(field).find('input').attr('checked', 'checked')
    //       else
    //         $(field).find('input').removeAttr('checked')

    //     submit: (field, annotation) ->
    //       checked = $(field).find('input').is(':checked')
    //       # Do something.
    //   })
    //
    // Returns the created <li> Element.
    addField: function(options) {
      var field = $.extend(
        {
          id: "annotator-field-" + id(),
          type: "input",
          label: "",
          load: function() {},
          submit: function() {}
        },
        options
      );

      var input = null,
        element = $('<li class="annotator-item" />');

      field.element = element[0];

      if (field.type === "textarea") {
        input = $("<textarea />");
      } else if (field.type === "checkbox") {
        input = $('<input type="checkbox" />');
      } else if (field.type === "input") {
        input = $("<input />");
      } else if (field.type === "select") {
        input = $("<select />");
      }

      element.append(input);

      input.attr({
        id: field.id,
        placeholder: field.label
      });

      if (field.type === "checkbox") {
        element.addClass("annotator-checkbox");
        element.append(
          $("<label />", {
            for: field.id,
            html: field.label
          })
        );
      }

      this.element.find("ul:first").append(element);
      this.fields.push(field);

      return field.element;
    },

    checkOrientation: function() {
      Widget.prototype.checkOrientation.call(this);

      var list = this.element.find("ul").first(),
        controls = this.element.find(".annotator-controls");

      if (this.element.hasClass(this.classes.invert.y)) {
        controls.insertBefore(list);
      } else if (controls.is(":first-child")) {
        controls.insertAfter(list);
      }

      return this;
    },

    // Event callback: called when a user clicks the editor form (by pressing
    // return, for example).
    //
    // Returns nothing
    _onFormSubmit: function(event) {
      preventEventDefault(event);
      this.submit();
    },

    // Event callback: called when a user clicks the editor's save button.
    //
    // Returns nothing
    _onSaveClick: function(event) {
      preventEventDefault(event);
      this.submit();
    },

    // Event callback: called when a user clicks the editor's cancel button.
    //
    // Returns nothing
    _onCancelClick: function(event) {
      preventEventDefault(event);
      this.cancel();
    }
  }));

  // Classes to toggle state.
  Editor.classes = {
    hide: "annotator-hide",
    focus: "annotator-focus"
  };

  // HTML template for this.element.
  Editor.template = [
    '<div class="annotator-outer annotator-editor annotator-touch-editor annotator-hide">',
    '  <div class="annotator-touch-widget">',
    '  <form class="annotator-widget annotator-touch-widget-inner">',
    '    <ul class="annotator-listing"></ul>',
    '    <div class="annotator-controls">',
    '     <a href="#cancel" class="annotator-cancel annotator-button">' +
      _t("Cancel") +
      "</a>",
    '      <a href="#save"',
    '         class="annotator-save annotator-focus annotator-button">' +
      _t("Save") +
      "</a>",
    "    </div>",
    "  </form>",
    "  </div>",
    "</div>"
  ].join("\n");

  // Configuration options
  Editor.options = {
    // Add the default field(s) to the editor.
    defaultFields: true
  };

  // standalone is a module that uses the Editor to display an editor widget
  // allowing the user to provide a note (and other data) before an annotation is
  // created or updated.
  window.annotator.touch.editor.standalone = function standalone(options) {
    var widget = new window.annotator.touch.editor.Editor(options);

    return {
      destroy: function() {
        widget.destroy();
      },
      beforeAnnotationCreated: function(annotation) {
        return widget.load(annotation);
      },
      beforeAnnotationUpdated: function(annotation) {
        return widget.load(annotation);
      }
    };
  };
})();
if (!window.annotator.touch) window.annotator.touch = {};
window.annotator.touch.xpath = {};

(function() {
  var $, Util;

  $ = window.annotator.util.$;

  Util = {};

  Util.NodeTypes = {
    ELEMENT_NODE: 1,
    ATTRIBUTE_NODE: 2,
    TEXT_NODE: 3,
    CDATA_SECTION_NODE: 4,
    ENTITY_REFERENCE_NODE: 5,
    ENTITY_NODE: 6,
    PROCESSING_INSTRUCTION_NODE: 7,
    COMMENT_NODE: 8,
    DOCUMENT_NODE: 9,
    DOCUMENT_TYPE_NODE: 10,
    DOCUMENT_FRAGMENT_NODE: 11,
    NOTATION_NODE: 12
  };

  Util.getFirstTextNodeNotBefore = function(n) {
    var result;
    switch (n.nodeType) {
      case Util.NodeTypes.TEXT_NODE:
        return n;
      case Util.NodeTypes.ELEMENT_NODE:
        if (n.firstChild != null) {
          result = Util.getFirstTextNodeNotBefore(n.firstChild);
          if (result != null) {
            return result;
          }
        }
        break;
    }
    n = n.nextSibling;
    if (n != null) {
      return Util.getFirstTextNodeNotBefore(n);
    } else {
      return null;
    }
  };

  Util.getLastTextNodeUpTo = function(n) {
    var result;
    switch (n.nodeType) {
      case Util.NodeTypes.TEXT_NODE:
        return n;
      case Util.NodeTypes.ELEMENT_NODE:
        if (n.lastChild != null) {
          result = Util.getLastTextNodeUpTo(n.lastChild);
          if (result != null) {
            return result;
          }
        }
        break;
    }
    n = n.previousSibling;
    if (n != null) {
      return Util.getLastTextNodeUpTo(n);
    } else {
      return null;
    }
  };

  Util.getTextNodes = function(jq) {
    var getTextNodes;
    getTextNodes = function(node) {
      var nodes;
      if (node && node.nodeType !== Util.NodeTypes.TEXT_NODE) {
        nodes = [];
        if (node.nodeType !== Util.NodeTypes.COMMENT_NODE) {
          node = node.lastChild;
          while (node) {
            nodes.push(getTextNodes(node));
            node = node.previousSibling;
          }
        }
        return nodes.reverse();
      } else {
        return node;
      }
    };
    return jq.map(function() {
      return Util.flatten(getTextNodes(this));
    });
  };

  Util.getGlobal = function() {
    return (function() {
      return this;
    })();
  };

  Util.contains = function(parent, child) {
    var node;
    node = child;
    while (node != null) {
      if (node === parent) {
        return true;
      }
      node = node.parentNode;
    }
    return false;
  };

  Util.flatten = function(array) {
    var flatten;
    flatten = function(ary) {
      var el, flat, _i, _len;
      flat = [];
      for (_i = 0, _len = ary.length; _i < _len; _i++) {
        el = ary[_i];
        flat = flat.concat(el && $.isArray(el) ? flatten(el) : el);
      }
      return flat;
    };
    return flatten(array);
  };

  window.annotator.touch.xpath.Util = Util;
}.call(this));

(function() {
  var $,
    Util,
    evaluateXPath,
    findChild,
    fromNode,
    getNodeName,
    getNodePosition,
    simpleXPathJQuery,
    simpleXPathPure,
    toNode;

  $ = window.annotator.util.$;

  Util = window.annotator.touch.xpath.Util;

  evaluateXPath = function(xp, root, nsResolver) {
    var exception, idx, name, node, step, steps, _i, _len, _ref;
    if (root == null) {
      root = document;
    }
    if (nsResolver == null) {
      nsResolver = null;
    }
    try {
      return document.evaluate(
        "." + xp,
        root,
        nsResolver,
        XPathResult.FIRST_ORDERED_NODE_TYPE,
        null
      ).singleNodeValue;
    } catch (_error) {
      exception = _error;
      console.log("XPath evaluation failed.");
      console.log("Trying fallback...");
      steps = xp.substring(1).split("/");
      node = root;
      for (_i = 0, _len = steps.length; _i < _len; _i++) {
        step = steps[_i];
        (_ref = step.split("[")), (name = _ref[0]), (idx = _ref[1]);
        idx =
          idx != null
            ? parseInt((idx != null ? idx.split("]") : void 0)[0])
            : 1;
        node = findChild(node, name.toLowerCase(), idx);
      }
      return node;
    }
  };

  simpleXPathJQuery = function($el, relativeRoot) {
    var jq;
    jq = $el.map(function() {
      var elem, idx, path, tagName;
      path = "";
      elem = this;
      while (
        (elem != null ? elem.nodeType : void 0) ===
          Util.NodeTypes.ELEMENT_NODE &&
        elem !== relativeRoot
      ) {
        tagName = elem.tagName.replace(":", "\\:");
        idx =
          $(elem.parentNode)
            .children(tagName)
            .index(elem) + 1;
        idx = "[" + idx + "]";
        path = "/" + elem.tagName.toLowerCase() + idx + path;
        elem = elem.parentNode;
      }
      return path;
    });
    return jq.get();
  };

  simpleXPathPure = function($el, relativeRoot) {
    var getPathSegment, getPathTo, jq, rootNode;
    getPathSegment = function(node) {
      var name, pos;
      name = getNodeName(node);
      pos = getNodePosition(node);
      return "" + name + "[" + pos + "]";
    };
    rootNode = relativeRoot;
    getPathTo = function(node) {
      var xpath;
      xpath = "";
      while (node !== rootNode) {
        if (node == null) {
          throw new Error(
            "Called getPathTo on a node which was not a descendant of @rootNode. " +
              rootNode
          );
        }
        xpath = getPathSegment(node) + "/" + xpath;
        node = node.parentNode;
      }
      xpath = "/" + xpath;
      xpath = xpath.replace(/\/$/, "");
      return xpath;
    };
    jq = $el.map(function() {
      var path;
      path = getPathTo(this);
      return path;
    });
    return jq.get();
  };

  findChild = function(node, type, index) {
    var child, children, found, name, _i, _len;
    if (!node.hasChildNodes()) {
      throw new Error("XPath error: node has no children!");
    }
    children = node.childNodes;
    found = 0;
    for (_i = 0, _len = children.length; _i < _len; _i++) {
      child = children[_i];
      name = getNodeName(child);
      if (name === type) {
        found += 1;
        if (found === index) {
          return child;
        }
      }
    }
    throw new Error("XPath error: wanted child not found.");
  };

  getNodeName = function(node) {
    var nodeName;
    nodeName = node.nodeName.toLowerCase();
    switch (nodeName) {
      case "#text":
        return "text()";
      case "#comment":
        return "comment()";
      case "#cdata-section":
        return "cdata-section()";
      default:
        return nodeName;
    }
  };

  getNodePosition = function(node) {
    var pos, tmp;
    pos = 0;
    tmp = node;
    while (tmp) {
      if (tmp.nodeName === node.nodeName) {
        pos += 1;
      }
      tmp = tmp.previousSibling;
    }
    return pos;
  };

  fromNode = function($el, relativeRoot) {
    var exception, result;
    try {
      result = simpleXPathJQuery($el, relativeRoot);
    } catch (_error) {
      exception = _error;
      console.log(
        "jQuery-based XPath construction failed! Falling back to manual."
      );
      result = simpleXPathPure($el, relativeRoot);
    }
    return result;
  };

  toNode = function(path, root) {
    var customResolver, namespace, node, segment;
    if (root == null) {
      root = document;
    }
    if (!$.isXMLDoc(document.documentElement)) {
      return evaluateXPath(path, root);
    } else {
      customResolver = document.createNSResolver(
        document.ownerDocument === null
          ? document.documentElement
          : document.ownerDocument.documentElement
      );
      node = evaluateXPath(path, root, customResolver);
      if (!node) {
        path = (function() {
          var _i, _len, _ref, _results;
          _ref = path.split("/");
          _results = [];
          for (_i = 0, _len = _ref.length; _i < _len; _i++) {
            segment = _ref[_i];
            if (segment && segment.indexOf(":") === -1) {
              _results.push(segment.replace(/^([a-z]+)/, "xhtml:$1"));
            } else {
              _results.push(segment);
            }
          }
          return _results;
        })().join("/");
        namespace = document.lookupNamespaceURI(null);
        customResolver = function(ns) {
          if (ns === "xhtml") {
            return namespace;
          } else {
            return document.documentElement.getAttribute("xmlns:" + ns);
          }
        };
        node = evaluateXPath(path, root, customResolver);
      }
      return node;
    }
  };

  window.annotator.touch.xpath.xpath = {
    fromNode: fromNode,
    toNode: toNode
  };
}.call(this));

(function() {
  var $,
    Range,
    Util,
    xpath,
    __hasProp = {}.hasOwnProperty,
    __extends = function(child, parent) {
      for (var key in parent) {
        if (__hasProp.call(parent, key)) child[key] = parent[key];
      }
      function ctor() {
        this.constructor = child;
      }
      ctor.prototype = parent.prototype;
      child.prototype = new ctor();
      child.__super__ = parent.prototype;
      return child;
    };

  xpath = window.annotator.touch.xpath.xpath;

  Util = window.annotator.touch.xpath.Util;

  $ = window.annotator.util.$;

  Range = {};

  Range.sniff = function(r) {
    if (r.commonAncestorContainer != null) {
      return new Range.BrowserRange(r);
    } else if (typeof r.start === "string") {
      return new Range.SerializedRange(r);
    } else if (r.start && typeof r.start === "object") {
      return new Range.NormalizedRange(r);
    } else {
      console.error("Could not sniff range type");
      return false;
    }
  };

  Range.RangeError = (function(_super) {
    __extends(RangeError, _super);

    function RangeError(type, message, parent) {
      this.type = type;
      this.message = message;
      this.parent = parent != null ? parent : null;
      RangeError.__super__.constructor.call(this, this.message);
    }

    return RangeError;
  })(Error);

  Range.BrowserRange = (function() {
    function BrowserRange(obj) {
      this.commonAncestorContainer = obj.commonAncestorContainer;
      this.startContainer = obj.startContainer;
      this.startOffset = obj.startOffset;
      this.endContainer = obj.endContainer;
      this.endOffset = obj.endOffset;
    }

    BrowserRange.prototype.normalize = function(root) {
      var nr, r;
      if (this.tainted) {
        console.error("You may only call normalize() once on a BrowserRange!");
        return false;
      } else {
        this.tainted = true;
      }
      r = {};
      this._normalizeStart(r);
      this._normalizeEnd(r);
      nr = {};
      if (r.startOffset > 0) {
        if (r.start.nodeValue.length > r.startOffset) {
          nr.start = r.start.splitText(r.startOffset);
        } else {
          nr.start = r.start.nextSibling;
        }
      } else {
        nr.start = r.start;
      }
      if (r.start === r.end) {
        if (nr.start.nodeValue.length > r.endOffset - r.startOffset) {
          nr.start.splitText(r.endOffset - r.startOffset);
        }
        nr.end = nr.start;
      } else {
        if (r.end.nodeValue.length > r.endOffset) {
          r.end.splitText(r.endOffset);
        }
        nr.end = r.end;
      }
      nr.commonAncestor = this.commonAncestorContainer;
      while (nr.commonAncestor.nodeType !== Util.NodeTypes.ELEMENT_NODE) {
        nr.commonAncestor = nr.commonAncestor.parentNode;
      }
      return new Range.NormalizedRange(nr);
    };

    BrowserRange.prototype._normalizeStart = function(r) {
      if (this.startContainer.nodeType === Util.NodeTypes.ELEMENT_NODE) {
        r.start = Util.getFirstTextNodeNotBefore(
          this.startContainer.childNodes[this.startOffset]
        );
        return (r.startOffset = 0);
      } else {
        r.start = this.startContainer;
        return (r.startOffset = this.startOffset);
      }
    };

    BrowserRange.prototype._normalizeEnd = function(r) {
      var n, node;
      if (this.endContainer.nodeType === Util.NodeTypes.ELEMENT_NODE) {
        node = this.endContainer.childNodes[this.endOffset];
        if (node != null) {
          n = node;
          while (n != null && n.nodeType !== Util.NodeTypes.TEXT_NODE) {
            n = n.firstChild;
          }
          if (n != null) {
            r.end = n;
            r.endOffset = 0;
          }
        }
        if (r.end == null) {
          if (this.endOffset) {
            node = this.endContainer.childNodes[this.endOffset - 1];
          } else {
            node = this.endContainer.previousSibling;
          }
          r.end = Util.getLastTextNodeUpTo(node);
          return (r.endOffset = r.end.nodeValue.length);
        }
      } else {
        r.end = this.endContainer;
        return (r.endOffset = this.endOffset);
      }
    };

    BrowserRange.prototype.serialize = function(root, ignoreSelector) {
      return this.normalize(root).serialize(root, ignoreSelector);
    };

    return BrowserRange;
  })();

  Range.NormalizedRange = (function() {
    function NormalizedRange(obj) {
      this.commonAncestor = obj.commonAncestor;
      this.start = obj.start;
      this.end = obj.end;
    }

    NormalizedRange.prototype.normalize = function(root) {
      return this;
    };

    NormalizedRange.prototype.limit = function(bounds) {
      var nodes, parent, startParents, _i, _len, _ref;
      nodes = $.grep(this.textNodes(), function(node) {
        return (
          node.parentNode === bounds || $.contains(bounds, node.parentNode)
        );
      });
      if (!nodes.length) {
        return null;
      }
      this.start = nodes[0];
      this.end = nodes[nodes.length - 1];
      startParents = $(this.start).parents();
      _ref = $(this.end).parents();
      for (_i = 0, _len = _ref.length; _i < _len; _i++) {
        parent = _ref[_i];
        if (startParents.index(parent) !== -1) {
          this.commonAncestor = parent;
          break;
        }
      }
      return this;
    };

    NormalizedRange.prototype.serialize = function(root, ignoreSelector) {
      var end, serialization, start;
      serialization = function(node, isEnd) {
        var n, nodes, offset, origParent, path, textNodes, _i, _len;
        if (ignoreSelector) {
          origParent = $(node)
            .parents(":not(" + ignoreSelector + ")")
            .eq(0);
        } else {
          origParent = $(node).parent();
        }
        path = xpath.fromNode(origParent, root)[0];
        textNodes = Util.getTextNodes(origParent);
        nodes = textNodes.slice(0, textNodes.index(node));
        offset = 0;
        for (_i = 0, _len = nodes.length; _i < _len; _i++) {
          n = nodes[_i];
          offset += n.nodeValue.length;
        }
        if (isEnd) {
          return [path, offset + node.nodeValue.length];
        } else {
          return [path, offset];
        }
      };
      start = serialization(this.start);
      end = serialization(this.end, true);
      return new Range.SerializedRange({
        start: start[0],
        end: end[0],
        startOffset: start[1],
        endOffset: end[1]
      });
    };

    NormalizedRange.prototype.text = function() {
      var node;
      return function() {
        var _i, _len, _ref, _results;
        _ref = this.textNodes();
        _results = [];
        for (_i = 0, _len = _ref.length; _i < _len; _i++) {
          node = _ref[_i];
          _results.push(node.nodeValue);
        }
        return _results;
      }
        .call(this)
        .join("");
    };

    NormalizedRange.prototype.textNodes = function() {
      var end, start, textNodes, _ref;
      textNodes = Util.getTextNodes($(this.commonAncestor));
      (_ref = [textNodes.index(this.start), textNodes.index(this.end)]),
        (start = _ref[0]),
        (end = _ref[1]);
      return $.makeArray(textNodes.slice(start, +end + 1 || 9e9));
    };

    return NormalizedRange;
  })();

  Range.SerializedRange = (function() {
    function SerializedRange(obj) {
      this.start = obj.start;
      this.startOffset = obj.startOffset;
      this.end = obj.end;
      this.endOffset = obj.endOffset;
    }

    SerializedRange.prototype.normalize = function(root) {
      var contains,
        e,
        length,
        node,
        p,
        range,
        targetOffset,
        tn,
        _i,
        _j,
        _len,
        _len1,
        _ref,
        _ref1;
      range = {};
      _ref = ["start", "end"];
      for (_i = 0, _len = _ref.length; _i < _len; _i++) {
        p = _ref[_i];
        try {
          node = xpath.toNode(this[p], root);
        } catch (_error) {
          e = _error;
          throw new Range.RangeError(
            p,
            "Error while finding " + p + " node: " + this[p] + ": " + e,
            e
          );
        }
        if (!node) {
          throw new Range.RangeError(
            p,
            "Couldn't find " + p + " node: " + this[p]
          );
        }
        length = 0;
        targetOffset = this[p + "Offset"];
        if (p === "end") {
          targetOffset -= 1;
        }
        _ref1 = Util.getTextNodes($(node));
        for (_j = 0, _len1 = _ref1.length; _j < _len1; _j++) {
          tn = _ref1[_j];
          if (length + tn.nodeValue.length > targetOffset) {
            range[p + "Container"] = tn;
            range[p + "Offset"] = this[p + "Offset"] - length;
            break;
          } else {
            length += tn.nodeValue.length;
          }
        }
        if (range[p + "Offset"] == null) {
          throw new Range.RangeError(
            "" + p + "offset",
            "Couldn't find offset " +
              this[p + "Offset"] +
              " in element " +
              this[p]
          );
        }
      }
      contains =
        document.compareDocumentPosition != null
          ? function(a, b) {
              return (
                a.compareDocumentPosition(b) &
                Node.DOCUMENT_POSITION_CONTAINED_BY
              );
            }
          : function(a, b) {
              return a.contains(b);
            };
      $(range.startContainer)
        .parents()
        .each(function() {
          var endContainer;
          if (range.endContainer.nodeType === Util.NodeTypes.TEXT_NODE) {
            endContainer = range.endContainer.parentNode;
          } else {
            endContainer = range.endContainer;
          }
          if (contains(this, endContainer)) {
            range.commonAncestorContainer = this;
            return false;
          }
        });
      return new Range.BrowserRange(range).normalize(root);
    };

    SerializedRange.prototype.serialize = function(root, ignoreSelector) {
      return this.normalize(root).serialize(root, ignoreSelector);
    };

    SerializedRange.prototype.toObject = function() {
      return {
        start: this.start,
        startOffset: this.startOffset,
        end: this.end,
        endOffset: this.endOffset
      };
    };

    return SerializedRange;
  })();

  window.annotator.touch.xpath.Range = Range;
}.call(this));
if (!window.annotator.touch) window.annotator.touch = {};
window.annotator.touch.textselector = {};

(function() {
  var Range = window.annotator.touch.xpath.Range;

  var $ = window.annotator.util.$;

  // isAnnotator determines if the provided element is part of Annotator. Useful
  // for ignoring mouse actions on the annotator elements.
  //
  // element - An Element or TextNode to check.
  //
  // Returns true if the element is a child of an annotator element.
  function isAnnotator(element) {
    var elAndParents = $(element)
      .parents()
      .addBack();
    return elAndParents.filter("[class^=annotator-]").length !== 0;
  }

  window.annotator.touch.textselector.isAnnotator = isAnnotator;

  // TextSelector monitors a document (or a specific element) for text selections
  // and can notify another object of a selection event
  function TextSelector(element, options) {
    this.element = element;
    this.options = $.extend(true, {}, TextSelector.options, options);
    this.onSelection = this.options.onSelection;
    this.timer = null;
    this.selectionString = "";
    this.hasSelection = false;

    if (
      typeof this.element.ownerDocument !== "undefined" &&
      this.element.ownerDocument !== null
    ) {
      var self = this;
      this.document = this.element.ownerDocument;

      this._watchForSelection();
    } else {
      console.warn(
        "You created an instance of the TextSelector on an " +
          "element that doesn't have an ownerDocument. This won't " +
          "work! Please ensure the element is added to the DOM " +
          "before the plugin is configured:",
        this.element
      );
    }
  }

  TextSelector.prototype.destroy = function() {
    if (this.timer) {
      window.cancelAnimationFrame(this.timer);
      this.timer = null;
    }
  };

  TextSelector.prototype._checkSelection = function() {
    var selection = window.getSelection();
    var previous = this.selectionString;
    var hasSelection = selection.rangeCount > 0;
    var hadSelection = this.hasSelection;
    var string = $.trim(selection + "");

    if (string == previous && hasSelection == hadSelection) {
      return;
    }

    this.selectionString = string;
    this.hadSelection = hasSelection;

    var self = this;

    var _nullSelection = function() {
      if (typeof self.onSelection === "function") {
        self.onSelection([]);
      }
    };

    if (
      selection.isCollapsed ||
      selection.rangeCount == 0 ||
      string.length == 0
    ) {
      _nullSelection();
      return;
    }

    var r = selection.getRangeAt(0),
      browserRange = new Range.BrowserRange(r),
      normedRange = browserRange.normalize().limit(this.element);

    // If the new range falls fully outside our this.element, we should
    // add it back to the document but not return it from this method.
    if (normedRange === null) {
      _nullSelection();
      return;
    }

    // Don't show the adder if the selection was of a part of Annotator itself.
    var container = normedRange.commonAncestor;
    if ($(container).hasClass("annotator-hl")) {
      container = $(container).parents("[class!=annotator-hl]")[0];
    }
    if (isAnnotator(container)) {
      _nullSelection();
      return;
    }

    if (typeof this.onSelection === "function") {
      this.onSelection([normedRange]);
    }
  };

  TextSelector.prototype._watchForSelection = function() {
    var self = this;

    if (this.timer) {
      return;
    }

    // There are occsions where Android will clear the text selection before
    // firing touch events. So we slow down the polling to ensure that touch
    // events get time to read the current selection.
    var interval = this._isAndroid() ? 300 : 1000 / 60;
    var start = new Date().getTime();

    // Use request animation frame despite the fact it runs to regularly to
    // take advantage of the fact it stops running when the window is idle.
    // If this becomes a performance bottleneck consider switching to a
    // longer setTimeout() and managing the start/stop manually.
    var step = function() {
      var progress = new Date().getTime() - start;
      if (progress > interval) {
        start = new Date().getTime();
        try {
          self._checkSelection();
        } catch (e) {
          console.error(e);
        }
      }
      self.timer = window.requestAnimationFrame(step);
    };
    step();
  };

  TextSelector.prototype._isAndroid = function() {
    return /Android/i.test(window.navigator.userAgent);
  };

  // Configuration options
  TextSelector.options = {
    // Callback, called when the user makes a selection.
    // Receives the list of selected ranges (may be empty).
    onSelection: null
  };

  window.annotator.touch.textselector.TextSelector = TextSelector;
})();
if (!window.annotator.touch) window.annotator.touch = {};
window.annotator.touch.viewer = {};

(function() {
  var Widget = window.annotator.ui.widget.Widget,
    util = window.annotator.util;

  var isAnnotator = window.annotator.touch.textselector.isAnnotator;

  var $ = util.$,
    _t = util.gettext;

  var NS = "annotator-viewer";

  // Private: simple parser for hypermedia link structure
  //
  // Examples:
  //
  //   links = [
  //     {
  //       rel: 'alternate',
  //       href: 'http://example.com/pages/14.json',
  //       type: 'application/json'
  //     },
  //     {
  //       rel: 'prev':
  //       href: 'http://example.com/pages/13'
  //     }
  //   ]
  //
  //   parseLinks(links, 'alternate')
  //   # => [{rel: 'alternate', href: 'http://...', ... }]
  //   parseLinks(links, 'alternate', {type: 'text/html'})
  //   # => []
  //
  function parseLinks(data, rel, cond) {
    cond = $.extend({}, cond, { rel: rel });

    var results = [];
    for (var i = 0, len = data.length; i < len; i++) {
      var d = data[i],
        match = true;

      for (var k in cond) {
        if (cond.hasOwnProperty(k) && d[k] !== cond[k]) {
          match = false;
          break;
        }
      }

      if (match) {
        results.push(d);
      }
    }

    return results;
  }

  // Public: Creates an element for viewing annotations.
  var Viewer = (window.annotator.touch.viewer.Viewer = Widget.extend({
    // Public: Creates an instance of the Viewer object.
    //
    // options - An Object containing options.
    //
    // Examples
    //
    //   # Creates a new viewer, adds a custom field and displays an annotation.
    //   viewer = new Viewer()
    //   viewer.addField({
    //     load: someLoadCallback
    //   })
    //   viewer.load(annotation)
    //
    // Returns a new Viewer instance.
    constructor: function(options) {
      Widget.call(this, options);

      this.itemTemplate = Viewer.itemTemplate;
      this.fields = [];
      this.annotations = [];
      this.render = function(annotation) {
        if (annotation.text) {
          return util.escapeHtml(annotation.text);
        } else {
          return "<i>" + _t("No comment") + "</i>";
        }
      };

      var self = this;

      if (this.options.defaultFields) {
        this.addField({
          load: function(field, annotation) {
            $(field).html(self.render(annotation));
          }
        });
      }

      if (typeof this.options.onEdit !== "function") {
        throw new TypeError("onEdit callback must be a function");
      }
      if (typeof this.options.onDelete !== "function") {
        throw new TypeError("onDelete callback must be a function");
      }
      if (typeof this.options.permitEdit !== "function") {
        throw new TypeError("permitEdit callback must be a function");
      }
      if (typeof this.options.permitDelete !== "function") {
        throw new TypeError("permitDelete callback must be a function");
      }

      if (this.options.autoViewHighlights) {
        this.document = this.options.autoViewHighlights.ownerDocument;

        $(this.options.autoViewHighlights).on(
          "tap." + NS,
          ".annotator-hl",
          function(event) {
            self._onHighlightTap(event);
          }
        );

        $(this.document).bind("tap." + NS, { preventDefault: false }, function(
          e
        ) {
          self._onDocumentTap(e);
        });
      }

      this.element
        .on("tap." + NS, ".annotator-edit", function(e) {
          self._onEditClick(e);
        })
        .on("tap." + NS, ".annotator-delete", function(e) {
          self._onDeleteClick(e);
        })
        .on("tap." + NS, ".annotator-item", function(e) {
          self._onTap(e);
        });
    },

    destroy: function() {
      if (this.options.autoViewHighlights) {
        $(this.options.autoViewHighlights).off("." + NS);
        $(this.document).off("." + NS);
      }
      this.element.off("." + NS);
      Widget.prototype.destroy.call(this);
    },

    // Public: Show the viewer.
    //
    // position - An Object specifying the position in which to show the editor
    //            (optional).
    //
    // Examples
    //
    //   viewer.show()
    //   viewer.hide()
    //   viewer.show({top: '100px', left: '80px'})
    //
    // Returns nothing.
    show: function(position) {
      if (typeof position !== "undefined" && position !== null) {
        this.element.css({
          top: position.top,
          left: position.left
        });
      }

      // var controls = this.element
      //   .find(".annotator-item")
      //   .addClass(this.classes.showControls);

      // var self = this;
      // setTimeout(function() {
      //   controls.removeClass(self.classes.showControls);
      // }, 500);

      Widget.prototype.show.call(this);
    },

    // Public: Load annotations into the viewer and show it.
    //
    // annotation - An Array of annotations.
    //
    // Examples
    //
    //   viewer.load([annotation1, annotation2, annotation3])
    //
    // Returns nothing.
    load: function(annotations, position) {
      this.annotations = annotations || [];

      var list = this.element.find("ul:first").empty();

      for (var i = 0, len = this.annotations.length; i < len; i++) {
        var annotation = this.annotations[i];
        this._annotationItem(annotation)
          .appendTo(list)
          .data("annotation", annotation);
      }

      this.show(position);
    },

    // Public: Set the annotation renderer.
    //
    // renderer - A function that accepts an annotation and returns HTML.
    //
    // Returns nothing.
    setRenderer: function(renderer) {
      this.render = renderer;
    },

    // Private: create the list item for a single annotation
    _annotationItem: function(annotation) {
      var item = $(this.itemTemplate).clone();

      var controls = item.find(".annotator-touch-controls"),
        link = controls.find(".annotator-link"),
        edit = controls.find(".annotator-edit"),
        del = controls.find(".annotator-delete");

      var links = parseLinks(annotation.links || [], "alternate", {
        type: "text/html"
      });
      var hasValidLink =
        links.length > 0 &&
        typeof links[0].href !== "undefined" &&
        links[0].href !== null;

      if (hasValidLink) {
        link.attr("href", links[0].href);
      } else {
        link.remove();
      }

      var controller = {};
      if (this.options.permitEdit(annotation)) {
        controller.showEdit = function() {
          edit.removeAttr("disabled");
        };
        controller.hideEdit = function() {
          edit.attr("disabled", "disabled");
        };
      } else {
        edit.remove();
      }
      if (this.options.permitDelete(annotation)) {
        controller.showDelete = function() {
          del.removeAttr("disabled");
        };
        controller.hideDelete = function() {
          del.attr("disabled", "disabled");
        };
      } else {
        del.remove();
      }

      for (var i = 0, len = this.fields.length; i < len; i++) {
        var field = this.fields[i];
        var element = $(field.element)
          .clone()
          .appendTo(item)[0];
        field.load(element, annotation, controller);
      }

      return item;
    },

    // Public: Adds an addional field to an annotation view. A callback can be
    // provided to update the view on load.
    //
    // options - An options Object. Options are as follows:
    //           load - Callback Function called when the view is loaded with an
    //                  annotation. Recieves a newly created clone of an item
    //                  and the annotation to be displayed (it will be called
    //                  once for each annotation being loaded).
    //
    // Examples
    //
    //   # Display a user name.
    //   viewer.addField({
    //     # This is called when the viewer is loaded.
    //     load: (field, annotation) ->
    //       field = $(field)
    //
    //       if annotation.user
    //         field.text(annotation.user) # Display the user
    //       else
    //         field.remove()              # Do not display the field.
    //   })
    //
    // Returns itself.
    addField: function(options) {
      var field = $.extend(
        {
          load: function() {}
        },
        options
      );

      field.element = $("<div />")[0];
      this.fields.push(field);
      return this;
    },

    // Event callback: called when the edit button is clicked.
    //
    // event - An Event object.
    //
    // Returns nothing.
    _onEditClick: function(event) {
      var item = $(event.target)
        .parents(".annotator-annotation")
        .data("annotation");
      this.hide();
      this.options.onEdit(item);
    },

    // Event callback: called when the delete button is clicked.
    //
    // event - An Event object.
    //
    // Returns nothing.
    _onDeleteClick: function(event) {
      var item = $(event.target)
        .parents(".annotator-annotation")
        .data("annotation");
      this.hide();
      this.options.onDelete(item);
    },

    // Event callback: called when a user triggers `tap` on a highlight
    // element.
    //
    // event - An Event object.
    //
    // Returns nothing.
    _onHighlightTap: function(event) {
      var annotations = $(event.target)
        .parents(".annotator-hl")
        .addBack()
        .map(function(_, elem) {
          return $(elem).data("annotation");
        })
        .toArray();

      var original = event.originalEvent;
      if (original && original.touches) {
        event.pageX = original.touches[0].pageX;
        event.pageY = original.touches[0].pageY;
      }

      // Now show the viewer with the wanted annotations
      this.load(annotations, util.mousePosition(event));
    },

    _onTap: function(event) {
      var target = $(event.currentTarget);
      var isVisible = target.hasClass(this.classes.showControls);
      this.element
        .find(".annotator-item")
        .removeClass(this.classes.showControls);
      if (!isVisible) {
        target.addClass(this.classes.showControls);
      }
    },

    _onDocumentTap: function(event) {
      if (!isAnnotator(event.target)) {
        this.hide();
      }
    }
  }));

  // Classes for toggling annotator state.
  Viewer.classes = {
    showControls: "annotator-visible"
  };

  // HTML templates for this.widget and this.item properties.
  Viewer.template = [
    '<div class="annotator-outer annotator-viewer annotator-hide annotator-touch-widget annotator-touch-viewer">',
    '  <ul class="annotator-widget annotator-listing"></ul>',
    "</div>"
  ].join("\n");

  Viewer.itemTemplate = [
    '<li class="annotator-annotation annotator-item">',
    '  <span class="annotator-touch-controls">',
    '    <a href="#"',
    '       title="' + _t("View as webpage") + '"',
    '       class="annotator-link">' + _t("View as webpage") + "</a>",
    '    <button type="button"',
    '            title="' + _t("Edit") + '"',
    '            class="annotator-edit annotator-button">' +
      _t("Edit") +
      "</button>",
    '    <button type="button"',
    '            title="' + _t("Delete") + '"',
    '            class="annotator-delete annotator-button">' +
      _t("Delete") +
      "</button>",
    "  </span>",
    "</li>"
  ].join("\n");

  // Configuration options
  Viewer.options = {
    // Add the default field(s) to the viewer.
    defaultFields: true,

    // Hook, passed an annotation, which determines if the viewer's "edit"
    // button is shown. If it is not a function, the button will not be shown.
    permitEdit: function() {
      return false;
    },

    // Hook, passed an annotation, which determines if the viewer's "delete"
    // button is shown. If it is not a function, the button will not be shown.
    permitDelete: function() {
      return false;
    },

    // If set to a DOM Element, will set up the viewer to automatically display
    // when the user hovers over Annotator highlights within that element.
    autoViewHighlights: null,

    // Callback, called when the user clicks the edit button for an annotation.
    onEdit: function() {},

    // Callback, called when the user clicks the delete button for an
    // annotation.
    onDelete: function() {}
  };

  // standalone is a module that uses the Viewer to display an viewer widget in
  // response to some viewer action (such as mousing over an annotator highlight
  // element).
  window.annotator.touch.viewer.standalone = function standalone(options) {
    var widget;

    if (typeof options === "undefined" || options === null) {
      options = {};
    }

    return {
      start: function(app) {
        var ident = app.registry.getUtility("identityPolicy");
        var authz = app.registry.getUtility("authorizationPolicy");

        // Set default handlers for what happens when the user clicks the
        // edit and delete buttons:
        if (typeof options.onEdit === "undefined") {
          options.onEdit = function(annotation) {
            app.annotations.update(annotation);
          };
        }
        if (typeof options.onDelete === "undefined") {
          options.onDelete = function(annotation) {
            app.annotations["delete"](annotation);
          };
        }

        // Set default handlers that determine whether the edit and delete
        // buttons are shown in the viewer:
        if (typeof options.permitEdit === "undefined") {
          options.permitEdit = function(annotation) {
            return authz.permits("update", annotation, ident.who());
          };
        }
        if (typeof options.permitDelete === "undefined") {
          options.permitDelete = function(annotation) {
            return authz.permits("delete", annotation, ident.who());
          };
        }

        widget = new window.annotator.touch.viewer.Viewer(options);
      },

      destroy: function() {
        widget.destroy();
      }
    };
  };
})();
if (!window.annotator.touch) window.annotator.touch = {};

(function() {
  var util = window.annotator.util;

  // Adds a new "tap" event to jQuery. This offers improved performance over
  // click for touch devices whcih often have up to a 300ms delay before
  // triggering callbacks. Instead it uses a combination of touchstart and
  // touchend events to to create a fake click. It will also cancel the event
  // after 300ms (to allow the user to perform a "longtap") or if the touchend
  // event is triggered on a different element.
  //
  // Additonal options can be provided as part of the eventData object.
  //
  // preventDefault - If false will not call preventDefault() on the touchstart
  //                  event (deafult: true).
  // onTapDown      - Callback for the "touchstart" incase additonal code needs
  //                  to be run such as event.stopPropagation().
  // onTapUp        - Callabck for the "touchend" event, called after the main
  //                  event handler.
  // timeout        - Time to allow before cancelling the event (default: 300).
  //
  // Example
  //
  //   jQuery("a").bind "tap", =>
  //     # This is called on "touchend" on the same element.
  //
  //   options =
  //     preventDefault: false
  //     onTapDown: (event) -> event.stopPropagation()
  //   jQuery(doument).delegate "button", "tap", options, =>
  //     # This is called on "touchend" on the same element.
  util.$.event.special.tap = {
    add: function(eventHandler) {
      var context, data, onTapEnd, onTapStart;
      data = eventHandler.data = eventHandler.data || {};
      context = this;
      onTapStart = function(event) {
        if (data.preventDefault !== false) event.preventDefault();
        if (data.onTapDown) data.onTapDown.apply(this, arguments);
        data.event = event;
        data.touched = setTimeout(function() {
          return (data.touched = null);
        }, data.timeout || 300);
        return jQuery(document).bind({
          touchend: onTapEnd,
          mouseup: onTapEnd
        });
      };
      onTapEnd = function(event) {
        var handler;
        if (data.touched != null) {
          clearTimeout(data.touched);
          if (
            event.target === context ||
            jQuery.contains(context, event.target)
          ) {
            handler = eventHandler.origHandler || eventHandler.handler;
            handler.call(this, data.event);
          }
          data.touched = null;
        }
        if (data.onTapUp) data.onTapUp.apply(this, arguments);
        return jQuery(document).unbind({
          touchstart: onTapEnd,
          mousedown: onTapEnd
        });
      };
      data.tapHandlers = {
        touchstart: onTapStart,
        mousedown: onTapStart
      };
      if (eventHandler.selector) {
        return jQuery(context).delegate(
          eventHandler.selector,
          data.tapHandlers
        );
      } else {
        return jQuery(context).bind(data.tapHandlers);
      }
    },
    remove: function(eventHandler) {
      return jQuery(this).unbind(eventHandler.data.tapHandlers);
    }
  };

  var adder = window.annotator.touch.adder;
  var enabler = window.annotator.touch.enabler;
  var editor = window.annotator.touch.editor;
  var highlighter = window.annotator.ui.highlighter;
  var textselector = window.annotator.touch.textselector;
  var viewer = window.annotator.touch.viewer;

  var _t = util.gettext;

  // trim strips whitespace from either end of a string.
  //
  // This usually exists in native code, but not in IE8.
  function trim(s) {
    if (typeof String.prototype.trim === "function") {
      return String.prototype.trim.call(s);
    } else {
      return s.replace(/^[\s\xA0]+|[\s\xA0]+$/g, "");
    }
  }

  // annotationFactory returns a function that can be used to construct an
  // annotation from a list of selected ranges.
  function annotationFactory(contextEl, ignoreSelector) {
    return function(ranges) {
      var text = [],
        serializedRanges = [];

      for (var i = 0, len = ranges.length; i < len; i++) {
        var r = ranges[i];
        text.push(trim(r.text()));
        serializedRanges.push(r.serialize(contextEl, ignoreSelector));
      }

      return {
        quote: text.join(" / "),
        ranges: serializedRanges
      };
    };
  }

  // maxZIndex returns the maximum z-index of all elements in the provided set.
  function maxZIndex(elements) {
    var max = -1;
    for (var i = 0, len = elements.length; i < len; i++) {
      var $el = util.$(elements[i]);
      if ($el.css("position") !== "static") {
        // Use parseFloat since we may get scientific notation for large
        // values.
        var zIndex = parseFloat($el.css("z-index"));
        if (zIndex > max) {
          max = zIndex;
        }
      }
    }
    return max;
  }

  // Helper function to inject CSS into the page that ensures Annotator elements
  // are displayed with the highest z-index.
  function injectDynamicStyle() {
    util.$("#annotator-dynamic-style").remove();

    var sel =
      "*" +
      ":not(annotator-adder)" +
      ":not(annotator-enabler)" +
      ":not(annotator-outer)" +
      ":not(annotator-notice)" +
      ":not(annotator-filter)";

    // use the maximum z-index in the page
    var max = maxZIndex(
      util
        .$(window.document.body)
        .find(sel)
        .get()
    );

    // but don't go smaller than 1010, because this isn't bulletproof --
    // dynamic elements in the page (notifications, dialogs, etc.) may well
    // have high z-indices that we can't catch using the above method.
    max = Math.max(max, 1000);

    var rules = [
      ".annotator-adder, .annotator-enabler, .annotator-outer, .annotator-notice {",
      "  z-index: " + (max + 20) + ";",
      "}",
      ".annotator-filter {",
      "  z-index: " + (max + 10) + ";",
      "}"
    ].join("\n");

    util
      .$("<style>" + rules + "</style>")
      .attr("id", "annotator-dynamic-style")
      .attr("type", "text/css")
      .appendTo("head");
  }

  // Helper function to remove dynamic stylesheets
  function removeDynamicStyle() {
    util.$("#annotator-dynamic-style").remove();
  }

  // Helper function to add permissions checkboxes to the editor
  function addPermissionsCheckboxes(editor, ident, authz) {
    function createLoadCallback(action) {
      return function loadCallback(field, annotation) {
        field = util.$(field).show();

        var u = ident.who();
        var input = field.find("input");

        // Do not show field if no user is set
        if (typeof u === "undefined" || u === null) {
          field.hide();
        }

        // Do not show field if current user is not admin.
        if (!authz.permits("admin", annotation, u)) {
          field.hide();
        }

        // See if we can authorise without a user.
        if (authz.permits(action, annotation, null)) {
          input.attr("checked", "checked");
        } else {
          input.removeAttr("checked");
        }
      };
    }

    function createSubmitCallback(action) {
      return function submitCallback(field, annotation) {
        var u = ident.who();

        // Don't do anything if no user is set
        if (typeof u === "undefined" || u === null) {
          return;
        }

        if (!annotation.permissions) {
          annotation.permissions = {};
        }
        if (
          util
            .$(field)
            .find("input")
            .is(":checked")
        ) {
          delete annotation.permissions[action];
        } else {
          // While the permissions model allows for more complex entries
          // than this, our UI presents a checkbox, so we can only
          // interpret "prevent others from viewing" as meaning "allow
          // only me to view". This may want changing in the future.
          annotation.permissions[action] = [authz.authorizedUserId(u)];
        }
      };
    }

    editor.addField({
      type: "checkbox",
      label: _t("Allow anyone to <strong>view</strong> this annotation"),
      load: createLoadCallback("read"),
      submit: createSubmitCallback("read")
    });

    editor.addField({
      type: "checkbox",
      label: _t("Allow anyone to <strong>edit</strong> this annotation"),
      load: createLoadCallback("update"),
      submit: createSubmitCallback("update")
    });
  }

  /**
   * function:: main([options])
   *
   * A module that provides a default user interface for Annotator that allows
   * users to create annotations by selecting text within (a part of) the
   * document.
   *
   * Example::
   *
   *     app.include(annotator.ui.main);
   *
   * :param Object options:
   *
   *   .. attribute:: options.element
   *
   *      A DOM element to which event listeners are bound. Defaults to
   *      ``document.body``, allowing annotation of the whole document.
   *
   *   .. attribute:: options.editorExtensions
   *
   *      An array of editor extensions. See the
   *      :class:`~annotator.ui.editor.Editor` documentation for details of editor
   *      extensions.
   *
   *   .. attribute:: options.viewerExtensions
   *
   *      An array of viewer extensions. See the
   *      :class:`~annotator.ui.viewer.Viewer` documentation for details of viewer
   *      extensions.
   *
   */
  window.annotator.touch.main = function(options) {
    if (typeof options === "undefined" || options === null) {
      options = {};
    }

    options.element = options.element || window.document.body;
    options.editorExtensions = options.editorExtensions || [];
    options.viewerExtensions = options.viewerExtensions || [];

    // Local helpers
    var makeAnnotation = annotationFactory(options.element, ".annotator-hl");

    // Object to hold local state
    var s = {};

    function start(app) {
      var ident = app.registry.getUtility("identityPolicy");
      var authz = app.registry.getUtility("authorizationPolicy");

      s.adder = new adder.Adder({
        onCreate: function(ann) {
          app.annotations.create(ann);
        }
      });
      s.adder.attach();

      if (!enabler.instance) {
        enabler.instance = new enabler.Enabler();
        enabler.instance.attach();
        enabler.instance.show();
      }

      s.enabled = enabler.instance.isEnabled();

      var toggleImgAreaSelect = function () {
        jQuery.each(jQuery(options.element).find("img"), function () {
          if (jQuery(this).data("imgAreaSelect")) {
            jQuery(this).data("imgAreaSelect").setOptions({disable: !s.enabled})
            jQuery(this).css({'cursor': s.enabled ? 'crosshair' : ''});
          }
        });
      }

      toggleImgAreaSelect();

      enabler.instance.onChange(function () {
        s.enabled = enabler.instance.isEnabled();
        toggleImgAreaSelect();
      });

      s.editor = new editor.Editor({
        extensions: options.editorExtensions
      });
      s.editor.attach();

      addPermissionsCheckboxes(s.editor, ident, authz);

      s.highlighter = new highlighter.Highlighter(options.element);

      s.textselector = new textselector.TextSelector(options.element, {
        onSelection: function(ranges) {
          if (s.enabled && ranges.length > 0) {
            var annotation = makeAnnotation(ranges);
            s.adder.load(annotation);
          } else {
            s.adder.hide();
          }
        }
      });

      s.viewer = new viewer.Viewer({
        onEdit: function(ann) {
          app.annotations.update(ann);
        },
        onDelete: function(ann) {
          app.annotations["delete"](ann);
        },
        permitEdit: function(ann) {
          return authz.permits("update", ann, ident.who());
        },
        permitDelete: function(ann) {
          return authz.permits("delete", ann, ident.who());
        },
        autoViewHighlights: options.element,
        extensions: options.viewerExtensions
      });
      s.viewer.attach();

      injectDynamicStyle();
    }

    return {
      configure: function(registry) {
        if (options.stylesheetUri) {
          util
            .$("<link />")
            .attr("rel", "stylesheet")
            .attr("type", "text/css")
            .attr("href", options.stylesheetUri)
            .appendTo("head");
        }

        // Some other modules, such as imgselect rely on `annotator.ui`. Swap
        // the adder, to make them touch-friendly.
        window.annotator.ui.adder = window.annotator.touch.adder;
      },

      start: start,

      destroy: function() {
        s.adder.destroy();
        s.editor.destroy();
        s.highlighter.destroy();
        s.textselector.destroy();
        s.viewer.destroy();
        removeDynamicStyle();
      },

      annotationsLoaded: function(anns) {
        s.highlighter.drawAll(anns);
      },
      annotationCreated: function(ann) {
        s.highlighter.draw(ann);
      },
      annotationDeleted: function(ann) {
        s.highlighter.undraw(ann);
      },
      annotationUpdated: function(ann) {
        s.highlighter.redraw(ann);
      },

      beforeAnnotationCreated: function(annotation) {
        // Editor#load returns a promise that is resolved if editing
        // completes, and rejected if editing is cancelled. We return it
        // here to "stall" the annotation process until the editing is
        // done.
        return s.editor.load(annotation);
      },

      beforeAnnotationUpdated: function(annotation) {
        return s.editor.load(annotation);
      }
    };
  };
})();
