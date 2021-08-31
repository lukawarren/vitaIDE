<template>
    <div>
        <prism-editor
            class="editor"
            v-model="code"
            :highlight="highlighter"
            :tabSize="4"
            v-on:input="runCode"
            line-numbers
        ></prism-editor>
    </div>
</template>

<script>
import { PrismEditor } from "vue-prism-editor";
import { highlight, languages } from "prismjs/components/prism-core";
import "prismjs/components/prism-lua";

export default {
    name: "Source",
    data: () => ({
        code: `-- Write your code here!
print("Hello world!")

for i=1,10 do
    print(i)
end
`,
    }),
    components: {
        PrismEditor,
    },
    methods: {
        highlighter(code) {
            return highlight(code, languages.lua);
        },

        runCode(code) {
            this.$emit("send", code);
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
