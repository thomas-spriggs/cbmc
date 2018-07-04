public class InnerClasses {
    public class PublicInnerClass {
        public int i;
        public PublicInnerClass(int i) {
            this.i = i;
        }
    }
    class DefaultInnerClass {
        int i;
        DefaultInnerClass(int i) {
            this.i = i;
        }
    }
    protected class ProtectedInnerClass {
        protected int i;
        protected ProtectedInnerClass(int i) {
            this.i = i;
        }
    }
    private class PrivateInnerClass {
        private int i;
        private PrivateInnerClass(int i) {
            this.i = i;
        }
    }
}

class InnerClassesDefault {
    public class PublicInnerClass {
        public int i;
        public PublicInnerClass(int i) {
            this.i = i;
        }
    }
    class DefaultInnerClass {
        int i;
        DefaultInnerClass(int i) {
            this.i = i;
        }
    }
    protected class ProtectedInnerClass {
        protected int i;
        protected ProtectedInnerClass(int i) {
            this.i = i;
        }
    }
    private class PrivateInnerClass {
        private int i;
        private PrivateInnerClass(int i) {
            this.i = i;
        }
    }
}

class InnerClassesDeeplyNested {
    class SinglyNestedClass {
        int i;
        SinglyNestedClass(int i) {
            this.i = i;
        }
        public class PublicDoublyNestedInnerClass {
            public int i;
            public PublicDoublyNestedInnerClass(int i) {
                this.i = i;
            }
        }
        class DefaultDoublyNestedInnerClass {
            int i;
            DefaultDoublyNestedInnerClass(int i) {
                this.i = i;
            }
        }
        protected class ProtectedDoublyNestedInnerClass {
            protected int i;
            protected ProtectedDoublyNestedInnerClass(int i) {
                this.i = i;
            }
        }
        private class PrivateDoublyNestedInnerClass {
            private int i;
            private PrivateDoublyNestedInnerClass(int i) {
                this.i = i;
            }
        }
    }
}

class ContainsAnonymousClass {
    interface InnerInterface {
        int i = 0;
    }
    public InnerInterface anonymousPublic = new InnerInterface() {
        int i = 1;
    };
    InnerInterface anonymousDefault = new InnerInterface() {
        int i = 2;
    };
    protected InnerInterface anonymousProtected = new InnerInterface() {
        int i = 3;
    };
    private InnerInterface anonymousPrivate = new InnerInterface() {
        int i = 4;
    };
}
