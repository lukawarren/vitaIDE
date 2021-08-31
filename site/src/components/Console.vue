<template>
    <div>
        <prism-editor
            class="editor"
            :value="code"
            :highlight="highlighter"
            :tabSize="4"
            :readonly="true"
        ></prism-editor>
    </div>
</template>

<script>
import { PrismEditor } from "vue-prism-editor";
import { highlight, languages } from "prismjs/components/prism-core";

export default {
    name: "Console",
    props: ["code"],
    components: {
        PrismEditor,
    },
    methods: {
        highlighter(code) {
            return highlight(code, languages.txt);
        },
    },
};
</script>

<style scoped lang="scss">
.editor {
    font-family: Fira code, Fira Mono, Consolas, Menlo, Courier, monospace;
    line-height: 1.5;
    color: white;
    max-height: 86vh;
    overflow: auto;
}

</style>

<style lang="css">
.prism-editor__textarea:focus {
    outline: none;
}

.prism-editor-wrapper .prism-editor__editor, .prism-editor-wrapper .prism-editor__textarea {
  white-space: pre !important;
}

.prism-editor-wrapper .prism-editor__container {
    overflow: visible !important;
}
</style>